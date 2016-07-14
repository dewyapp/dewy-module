<?php

namespace Drupal\dewy\Form;

use Drupal\Component\Utility\Unicode;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * Configure site information settings for this site.
 */
class DewyConfig extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'dewy_config';
  }

  protected function getEditableConfigNames() {
    return ['dewy.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('dewy.settings');

    $form['dewy_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Dewy to report on this site'),
      '#default_value' => $config->get('enabled'),
      '#description' => t('Enable Dewy to communicate with this site. Turning this off prevents Dewy from monitoring your site.')
    );
    $form['dewy_users'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include usernames and emails'),
      '#default_value' => $config->get('users'),
      '#description' => t('Allow Dewy to pull username and email data.')
    );
    $form['dewy_content'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include content'),
      '#default_value' => $config->get('content'),
      '#description' => t('Allow Dewy to pull content data.')
    );
    $form['dewy_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Dewy API key'),
      '#default_value' => $config->get('api_key'),
      '#size' => 40,
      '#maxlength' => 36,
      '#description' => t('The API key of your Dewy account. This is required to link this site to your Dewy account. <a href="http://dewy.io">Get a Dewy API key</a>.'),
      '#required' => TRUE
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('dewy.settings');

    global $base_url;

    // if ($form_state['values']['dewy_enabled'] && !variable_get('dewy_token', 0)) {
    //     DewyToken::createToken();
    // } 
    // else if (!$form_state['values']['dewy_enabled']) {
    //     DewyToken::deleteToken();
    // }

    // Test that the site URL is available from the web
    $options = array(
        'method' => 'POST',
        'data' => 'token='.$config->get('token'),
        'timeout' => 15,
        'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );

    $client = \Drupal::httpClient();
    $request = $client->request('POST', $base_url.'/admin/reports/dewy', [
      'http_errors' => false,
      'form_params' => [
        'token' => $config->get('token')
      ]
    ]);
    if ($request->getStatusCode() != '200' && $form_state->getValue('dewy_enabled')) {
      $form_state->setErrorByName('dewy_api_key', t('This site is not reachable at the base_url provided ('.$base_url.'). The base_url can be specified in this site\'s settings.php file, or if using Drush, the \'--uri=SITE\' flag.'));
    }
    else {
      // Attempt to register the site with Dewy
      $request = $client->request('POST', 'http://api.dewy.io/1.0/sites', [
        'http_errors' => false,
        'form_params' => [
          'apikey' => $form_state->getValue('dewy_api_key'),
          'token' => $config->get('token'),
          'baseurl' => $base_url,
          'enabled' => $form_state->getValue('dewy_enabled'),
          'read_users' => $form_state->getValue('dewy_users'),
          'read_content' => $form_state->getValue('dewy_content')
        ]
      ]);
      if ($request->getStatusCode() != '200') {
        if ($request->getStatusCode() == '500' || $request->getStatusCode() == '503') {
          $form_state->setErrorByName('dewy_api_key', t('Dewy is unreachable, the API key cannot be verified at this time.'));
        }
        else {
          $form_state->setErrorByName('dewy_api_key', $request->getBody());
        }
      }
    }
    
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    parent::submitForm($form, $form_state);
  }

}
