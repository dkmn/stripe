<?php

/**
 * @file
 * Stripe administration and module settings UI.
 */

/**
 * Menu callback: configure Stripe API Keys
 */
function stripe_admin_keys() {
  $keys = array(
    'test_secret' => t('Test Secret Key'),
    'test_publishable' => t('Test Publishable Key'),
    'live_secret' => t('Live Secret Key'),
    'live_publishable' => t('Live Publishable Key'),
  );
  
  $form['api_keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('API Keys'),
    '#collapsible' => FALSE,
  );

  $active_key = variable_get('stripe_key_status', 'test');
  
  $form['api_keys']['stripe_key_status'] = array(
    '#type' => 'radios',
    '#title' => t('Stripe API Status'),
    '#default_value' => $active_key,
    '#options' => array(
      'test' => t('Test'),
      'live' => t('Live')
    ),
    '#description' => t('This determines which set of keys you are using.'),
  );
  
  foreach ($keys as $machine_name => $name) {
    $form['api_keys']['stripe_' . $machine_name] = array(
      '#type' => 'textfield',
      '#title' => $name,
      '#size' => 35,
      '#default_value' => variable_get('stripe_' . $machine_name, ''),
    );
  }
  return system_settings_form($form);
}
