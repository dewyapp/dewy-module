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
  
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    parent::submitForm($form, $form_state);
  }

}
