<?php

/**
 * @file
 * Processes path counts for Dewey
 */

/**
* Root directory of Drupal installation.
*/
define('DRUPAL_ROOT', substr($_SERVER['SCRIPT_FILENAME'], 0, strpos($_SERVER['SCRIPT_FILENAME'], 'sites')));
// Change the directory to the Drupal root.
chdir(DRUPAL_ROOT);

include_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_VARIABLES);
$path = '/';
if (isset($_POST['path'])) {
  $path = $_POST['path'];
}
db_merge('dewey_counter')
  ->key(array('path' => $path))
  ->fields(array(
    'count' => 1,
  ))
  ->expression('count', 'count + 1')
  ->execute();