<?php

    function dewy_drush_command() {
        $items = array();

        $items['dewy-enable'] = array(
            'description' => dt('Enable site reporting to Dewy.'),
            'arguments' => array(
                'api_key'        => 'API key for your Dewy account.',
            ),
            'options' => array(
                'read-users' => 'Flag to allow user information to be read',
                'read-content' => 'Flag to allow site content to be read',
            ),
            'examples' => array(
                'drush dewy-enable api_key' => 'Enables site reporting to your Dewy account with your API key.',
                'drush dewy-enable api_key --read-users --read-content' => 'Enables Dewy and allows users and content information to be read.',
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
        // Get api key and options
        list($api_key) = func_get_args();

        $read_users = 0;
        if (drush_get_option('read-users')) {
            $read_users = 1;
        }
        $read_content = 0;
        if (drush_get_option('read-content')) {
            $read_content = 1;
        }

        dewy_register($api_key, 1, $read_users, $read_content);
    }

    function drush_dewy_disable() {
        dewy_register(null, 0, null, null);
    }