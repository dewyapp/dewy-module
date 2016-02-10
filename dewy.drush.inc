<?php

    function dewy_drush_command() {
        $items = array();

        $items['dewy-enable'] = array(
            'description' => dt('Enable site reporting to Dewy.'),
            'arguments' => array(
                'api_key'        => 'API key for your Dewy account.',
            ),
            'options' => array(
                'read_users' => '',
                'read_content' => '',
            ),
            'examples' => array(
                'drush dewy-enable api_key' => 'Enables site reporting to your Dewy account with your API key.',
            ),
            'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
        );

        $items['dewy-disable'] = array(
            'description' => dt('Disable site reporting to Dewy.'),
            'examples' => array(
                'drush dewy-disable' => 'Disables site reporting to Dewy.',
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
        dewy_register($api_key, 1, 1, 1);
    }

    function drush_dewy_disable() {
        dewy_register(null, 0, null, null);
    }