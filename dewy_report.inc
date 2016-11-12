<?php

/**
 * @file
 * Code for the Dewy report.
 */

/**
 * Dewy report.
 */
function dewy_report() {
  // If PHP stops execution at anything under 10 minutes, attempt to
  // set time to 10 minutes.
  if (ini_get('max_execution_time') > 0 && ini_get('max_execution_time') < 600) {
    set_time_limit(600);
  }

  // Measure benchmark.
  $time_start = microtime(TRUE);

  // Open stream.
  $stream = new SplFileObject('php://output', 'w');
  $stream->fwrite('{');

  // Get audit time.
  $stream->fwrite('"date":' . json_encode(time()) . ',');

  // Get title.
  $stream->fwrite('"title":' . json_encode(variable_get('site_name')) . ',');

  // Get paths.
  $stream->fwrite('"base_url":' . json_encode($GLOBALS['base_url']) . ',');

  // Get Drupal version.
  $stream->fwrite('"drupal_core":' . json_encode(VERSION) . ',');

  // Get PHP details.
  $stream->fwrite('"php":' . json_encode(array(
    'version' => PHP_VERSION,
    'max_execution_time' => ini_get('max_execution_time'),
    'memory_limit' => ini_get('memory_limit'),
    ),
  ) . ',',
  );

  // Get path accesses.
  if (variable_get('dewy_traffic', 1)) {
    $stream->fwrite('"traffic":{');
    $ignores = array('admin');
    $result = db_query("SELECT path, hits, lastaccess FROM dewy_counter");
    $stream->fwrite('"recorded_since":' . json_encode(variable_get('dewy_count_date')) . ',');
    $stream->fwrite('"paths":{');
    $first_row = TRUE;
    while ($path = $result->fetchAssoc()) {
      foreach ($ignores as $ignore) {
        if (substr($path['path'], 0, strlen($ignore)) != $ignore) {
          if (!$first_row) {
            $stream->fwrite(',');
          }
          else {
            $first_row = FALSE;
          }
          $stream->fwrite(json_encode($path['path']) . ':' . json_encode(array('hits' => (float) $path['hits'], 'last_access' => (float) $path['lastaccess'])));
        }
      }
    }
    $stream->fwrite('}},');
  }

  // Get the file count and file sizes.
  $stream->fwrite('"files":{');
  $types = array('public', 'private');
  $path['public'] = variable_get('file_public_path', conf_path() . '/files');
  $path['private'] = variable_get('file_private_path');

  $first_row = TRUE;
  foreach ($types as $type) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }

    $count = 0;
    $size = 0;

    if (is_dir($path[$type])) {
      $iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path[$type], RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
      foreach ($iterator as $file) {
        if (!$file->isDir()) {
          $count++;
          $size += number_format($file->getSize() / 1048576, 2);
        }
      }
    }
    $stream->fwrite(json_encode($type) . ':' . json_encode(array('count' => $count, 'size' => $size)));
  }
  $stream->fwrite('},');

  // Get database file size.
  $db_size = 0;
  try {
    $database = $GLOBALS['databases']['default']['default']['database'];
    $db_size = db_query("SELECT Round(Sum(data_length + index_length) / 1024 / 1024, 1) as 'mb' FROM information_schema.tables WHERE table_schema = :database", array(':database' => $database))->fetchField();
  }
  catch (Exception $e) {
  }
  $stream->fwrite('"db_size":' . json_encode((float) $db_size) . ',');

  // Get all users, roles and last signons.
  $stream->fwrite('"users":{');
  $result = db_query("SELECT users.name AS name, mail, created, access AS last_access, status, GROUP_CONCAT(role.name SEPARATOR ',') AS roles FROM users, role, users_roles WHERE users.uid = users_roles.uid AND users_roles.rid = role.rid GROUP BY name");
  $first_row = TRUE;
  $i = 0;
  while ($user = $result->fetchAssoc()) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }
    // Obfuscate usernames and email addresses if necessary.
    if (!variable_get('dewy_users', 1)) {
      $i++;
      $user['name'] = "$i";
      $user['mail'] = '-1';
    }
    $stream->fwrite(json_encode($user['name']) . ':' . json_encode(array(
      'mail' => $user['mail'],
      'created' => (float) $user['created'],
      'last_access' => (float) $user['last_access'],
      'status' => $user['status'],
      'roles' => explode(',', $user['roles']),
      ),
    ));
  }
  $stream->fwrite('},');

  // Get all text fields on the site.
  $text_fields = array();
  $fields = field_info_fields();
  foreach ($fields as $field) {
    if ($field['module'] == 'text') {
      array_push($text_fields, $field);
    }
  }
  unset($fields);

  // Get all node metadata.
  $stream->fwrite('"nodes":{');

  $query = db_select('node', 'n');
  $query->fields('n', array('nid', 'type', 'created', 'changed', 'status'));
  foreach ($text_fields as $field) {
    $table = _field_sql_storage_tablename($field);
    $column = _field_sql_storage_columnname($field['field_name'], 'value');
    $query->leftJoin($table, NULL, "$table.entity_id = n.nid");
    $query->fields(NULL, array($column));
  }
  $result = $query->execute();

  $first_row = TRUE;
  while ($node = $result->fetchAssoc()) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }

    $words = 0;
    $data = array('content' => array());
    foreach ($node as $key => $value) {
      // If key is a node value, store it.
      if ($key == 'nid' || $key == 'type' || $key == 'created' || $key == 'changed' || $key == 'status') {
        $data[$key] = $value;
      }
      // Otherwise, the key is a †ext field and the value is content.
      else {
        $words = $words + str_word_count($value, 0);
      }
    }

    $stream->fwrite(json_encode($data['nid']) . ':' . json_encode(array(
      'created' => (float) $data['created'],
      'changed' => (float) $data['changed'],
      'status' => (int) $data['status'],
      'type' => $data['type'],
      'words' => $words,
      ),
    ));
  }
  $stream->fwrite('},');
  unset($text_fields);

  // Get all block content.
  $stream->fwrite('"blocks":{');
  $result = db_select('block_custom', 'block')
    ->fields('block', array('bid', 'body'))
    ->execute();
  $first_row = TRUE;
  while ($block = $result->fetchAssoc()) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }

    $stream->fwrite(json_encode($block['bid']) . ':' . json_encode(array('words' => str_word_count($block['body'], 0))));
  }
  $stream->fwrite('},');

  // Get all projects and their respective modules/schema updates.
  $stream->fwrite('"projects":');
  require_once DRUPAL_ROOT . '/includes/install.inc';
  drupal_load_updates();
  $modules = system_rebuild_module_data();
  $projects = array();
  foreach ($modules as $module => $module_info) {
    $schemas = drupal_get_schema_versions($module_info->name);
    $latest_schema = 0;
    if ($schemas !== FALSE) {
      $latest_schema = max($schemas);
    }
    // Unfortunately I need to show hidden modules, because when you
    // are logged out, many essential modules are set to hidden (that aren't
    // set to hidden when logged in) for a reason I haven't figured out yet.
    // if (!$module_info->info['hidden']) {
    // Instead we can hide modules that are core and under package 'Testing',
    // this seems to hide the modules we don't care about.
    if ($module_info->info['project'] != "Drupal" && $module_info->info['package'] != "Testing") {
      if (!isset($projects[$module_info->info['project']])) {
        $projects[$module_info->info['project']] =
          array(
            'modules' => array(
              $module => array(
                'schema' => (float) $module_info->schema_version,
                'latest_schema' => (float) $latest_schema,
                'dependencies' => $module_info->info['dependencies'],
              ),
            ),
            'version' => $module_info->info['version'],
          );
      }
      else {
        $projects[$module_info->info['project']]['modules'][$module] = array(
          'schema' => (float) $module_info->schema_version,
          'latest_schema' => (float) $latest_schema,
          'dependencies' => $module_info->info['dependencies'],
        );
      }
    }
  }
  $stream->fwrite(json_encode($projects));
  $stream->fwrite(',');
  unset($modules);
  unset($projects);

  // Get all custom themes.
  $stream->fwrite('"themes":{');
  $themes = list_themes(TRUE);
  $first_row = TRUE;
  foreach ($themes as $theme) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }
    $stream->fwrite(json_encode($theme->name) . ':' . json_encode(array('version' => $theme->info['version'], 'status' => $theme->status)));
  }
  $stream->fwrite('},');
  unset($themes);

  // Get all variables.
  $stream->fwrite('"variables":{');
  $result = db_query("SELECT name FROM variable");
  $first_row = TRUE;
  while ($variable = $result->fetchAssoc()) {
    if (!$first_row) {
      $stream->fwrite(',');
    }
    else {
      $first_row = FALSE;
    }
    $stream->fwrite(json_encode($variable['name']) . ':' . json_encode(variable_get($variable['name'])));
  }
  $stream->fwrite('},');

  // Report benchmark.
  $time_end = microtime(TRUE);
  $time = $time_end - $time_start;
  $stream->fwrite('"benchmark":' . $time);

  // Close stream.
  $stream->fwrite('}');
}
