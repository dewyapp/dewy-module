<?php

function dewey_drush_command() {
  $items = array();

  $items['dewey-enable'] = array(
    'description' => dt('Enable site reporting to Dewey.'),
    'arguments' => array(
      'api_key'    => 'API key for your Dewey account.',
    ),
    'examples' => array(
      'drush dewey-enable P17G823F724A98238QT' => 'Enable site reporting to your Dewey account with your API key.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
  );

  return $items;
}

function drush_dewey_enable_validate() {
  $args = func_get_args();
  if (empty($args[0])) {
    drush_set_error('NO_TEXT', dt('An API key for your Dewey account must be provided. Type "drush help dewey-enable" for help.'));
  }
}

function drush_dewey_enable() {
  list($api_key) = func_get_args();
	drush_log('Enabled site reporting to Dewey.', 'success');
}