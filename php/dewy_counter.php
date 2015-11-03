<?php

/**
 * @file
 * Processes path counts for Dewy
 */

/**
* Root directory of Drupal installation.
*/
if (isset($_POST['drupal'])) {
  define('DRUPAL_ROOT', $_POST['drupal']);
  // Change the directory to the Drupal root.
  chdir($_POST['drupal']);

  include_once $_POST['drupal'] . '/includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_VARIABLES);
  $path = '/';
  if (isset($_POST['path'])) {
    $path = $_POST['path'];
  }
  db_merge('dewy_counter')
    ->key(array('path' => $path))
    ->fields(array(
      'hits' => 1,
      'lastaccess' => REQUEST_TIME,
    ))
    ->expression('hits', 'hits + 1')
    ->execute();
}