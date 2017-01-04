<?php

/**
 * @file
 * Code for the Dewy content report.
 */

/**
 * Dewy content report.
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

  // Get all text fields on the site.
  $text_fields = array();
  $fields = field_info_fields();
  foreach ($fields as $field) {
    if ($field['module'] == 'text') {
      $table = _field_sql_storage_tablename($field);
      $column = _field_sql_storage_columnname($field['field_name'], 'value');
      $text_fields[$table] = $column;
    }
  }
  unset($fields);

  // Get all node content.
  $stream->fwrite('"nodes":{');

  $queries = array();
  // There's a hard limit in MySQL of 61 joins per query,
  // so we will join in buckets of 50.
  $number_of_buckets = ceil(sizeof($text_fields)/50);
  for ($bucket = 0; $bucket < $number_of_buckets; $bucket++) {
    $query = db_select('node', 'n');
    $query->fields('n', array('nid'));
    $text_field_count = 0;
    // For each bucket we need to replicate all the same
    // columns so that a UNION JOIN can be done across
    // all queries. If the same columns aren't replicated,
    // we will run into a mismatched columns error.
    foreach ($text_fields as $table => $column) {
      // Active bucket, do the joins.
      if ($text_field_count >= $bucket * 50 && 
          $text_field_count < ($bucket + 1) * 50) {
        $query->leftJoin($table, NULL, "$table.entity_id = n.nid");
        $query->addField($table, $column);
      }
      // Otherwise just replicate columns.
      else {
        $query->addField('n', 'nid', $column);
      }
      $text_field_count++;
    }
    array_push($queries, $query);
  }

  $subquery = $queries[0];
  $queriesPerNode = sizeof($queries);
  // If there were multiple queries because we had
  // too many joins, union them all.
  for ($i = 1; $i < $queriesPerNode; $i++) {
    $subquery->union($queries[$i], 'UNION ALL');
  }
  $query = db_select('node', 'n');
  // Order the unioned results by nid so that
  // totals can be added sequentially.
  $query = db_select($subquery,'subquery')->fields('subquery')->orderBy('nid');
  $result = $query->execute();

  $first_row = TRUE;
  $currentQuery = 1;
  $data = array('content' => array());
  while ($node = $result->fetchAssoc()) {
    foreach ($node as $key => $value) {
      // If key is a node value, store it.
      if ($key == 'nid') {
        $data[$key] = $value;
      }
      // Otherwise, the key is a †ext field and the value is content.
      elseif ($value) {
        $data['content'][$key] = $value;
      }
    }

    // Once all results per node have been parsed,
    // write the output.
    // In most cases there will be one result
    // per node, but when the query is split up
    // due to MySQL join limits, there may be many.
    if ($currentQuery == $queriesPerNode) {
      if (!$first_row) {
        $stream->fwrite(',');
      }
      else {
        $first_row = FALSE;
      }
      $stream->fwrite(json_encode($data['nid']) . ':' . json_encode($data['content']));
      // Reset totals.
      $data = array('content' => array());
      $currentQuery = 1;
    }
    else {
      $currentQuery++;
    }
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

    $stream->fwrite(json_encode($block['bid']) . ':' . json_encode($block['body']));
  }
  $stream->fwrite('},');

  // Report benchmark.
  $time_end = microtime(TRUE);
  $time = $time_end - $time_start;
  $stream->fwrite('"benchmark":' . $time);

  // Close stream.
  $stream->fwrite('}');
}
