<?php

/**
 * @file
 * Processes path counts for Dewey
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
  db_merge('dewey_counter')
    ->key(array('path' => $path))
    ->fields(array(
      'count' => 1,
    ))
    ->expression('count', 'count + 1')
    ->execute();
}