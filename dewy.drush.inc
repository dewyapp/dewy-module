<?php

function dewy_drush_command() {
  $items = array();

  $items['dewy-enable'] = array(
    'description' => dt('Enable site reporting to Dewy.'),
    'arguments' => array(
      'api_key'    => 'API key for your Dewy account.',
    ),
    'examples' => array(
      'drush dewy-enable api_key' => 'Enable site reporting to your Dewy account with your API key.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
  );

  return $items;
}

function drush_dewy_enable_validate() {
  $args = func_get_args();
  if (empty($args[0])) {
    drush_set_error('NO_TEXT', dt('An API key for your Dewy account must be provided. Type "drush help dewy-enable" for help.'));
  }
}

function drush_dewy_enable() {
  list($api_key) = func_get_args();
	drush_log('Enabled site reporting to Dewy.', 'success');
}