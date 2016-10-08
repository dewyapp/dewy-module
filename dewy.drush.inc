<?php

    function dewy_drush_command() {
        $items = array();

        $items['dewy-enable'] = array(
            'description' => dt('Enable site reporting to Dewy.'),
            'arguments' => array(
                'api_key'        => 'API key for your Dewy account.',
            ),
            'options' => array(
                'hide-users' => 'Flag to prevent Dewy from reading user information',
                'hide-content' => 'Flag to prevent Dewy from reading site content',
                'hide-traffic' => 'Flag to prevent generation of site traffic data for Dewy',
            ),
            'examples' => array(
                'drush dewy-enable api_key' => 'Enables site reporting to your Dewy account with your API key.',
                'drush dewy-enable api_key --hide-users --hide-content' => 'Enables Dewy and prevents users and content information to be read.',
            ),
            'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
        );

        $items['dewy-disable'] = array(
            'description' => dt('Disable site reporting to Dewy.'),
            'examples' => array(
                'drush dewy-disable' => 'Disables site reporting to Dewy.',
            ),
            'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
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
        // Get options and build form state
        $form_state = array();
        $form_state['values']['dewy_enabled'] = 1;

        list($api_key) = func_get_args();
        $form_state['values']['dewy_api_key'] = $api_key;

        $form_state['values']['dewy_users'] = 1;
        if (drush_get_option('hide-users')) {
            $form_state['values']['dewy_users'] = NULL;
        }
        $form_state['values']['dewy_content'] = 1;
        if (drush_get_option('hide-content')) {
            $form_state['values']['dewy_content'] = NULL;
        }
        $form_state['values']['dewy_traffic'] = 1;
        if (drush_get_option('hide-traffic')) {
            $form_state['values']['dewy_traffic'] = NULL;
        }

        // Submit form
        drupal_form_submit('dewy_config', $form_state);
    }

    function drush_dewy_disable() {
        // Build form state
        $form_state = array();
        $form_state['values']['dewy_enabled'] = NULL;
        $form_state['values']['dewy_api_key'] = variable_get('dewy_api_key');

        $form_state['values']['dewy_users'] = NULL;
        if (variable_get('dewy_users')) {
            $form_state['values']['dewy_users'] = 1;
        }
        $form_state['values']['dewy_content'] = NULL;
        if (variable_get('dewy_content')) {
            $form_state['values']['dewy_content'] = 1;
        }
        $form_state['values']['dewy_traffic'] = NULL;
        if (variable_get('dewy_traffic')) {
            $form_state['values']['dewy_traffic'] = 1;
        }

        // Submit form
        drupal_form_submit('dewy_config', $form_state);
    }