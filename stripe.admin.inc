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

function stripe_admin_test() {
  $form = stripe_form_default_items();
  $form['card_number']['#description'] = t('Use 4242424242424242 for testing');
  $form['card_cvc']['#description'] = t('Use 123 for testing');
  $form['card_expiry_month']['#default_value'] = '10';
  $form['card_expiry_year']['#default_value'] = '2020';
  $form['amount'] = array(
     '#type' => 'textfield',
     '#title' => t('Amount:'),
     '#description' => '<em>cents</em>',
     '#default_value' => 99,
     '#size' => 6,
     '#attributes' => array(
       'class' => array('amount'),
     ),
     '#weight' => 2,
   );
   $form['card_submit']['#weight'] = 3;
  return $form;
}



function stripe_admin_test_submit($form, &$form_state) {
  require_once("sites/all/libraries/stripe/lib/Stripe.php");
  
  $status = variable_get('stripe_key_status', 'test');
  $secret_key_name = 'stripe_' . $status . '_secret';
  $secret_key = variable_get($secret_key_name, '');
  
  Stripe::setApiKey($secret_key);
  
  $charge = Stripe_Charge::create(array(
    "amount" => $form_state['values']['amount'], // amount in cents, again
    "currency" => "usd",
    "card" => $form_state['values']['stripe_token'],
    "description" => "Test Charge from " . variable_get('site_name', 'My Drupal Site'))
  );
  //TODO Check on error handling here;
  drupal_set_message("Success! Card was successfully charged for the amount of " . check_plain($form_state['values']['amount']));
}



/**
 * Helper function to go fetch the normal form elements.
 */
function stripe_form_default_items() {
  $form = array();
  $form['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Card Number:'),
    '#size' => 16,
    '#attributes' => array(
      'autocomplete' => 'off',
      'class' => array('card-number'),
    ),
    '#disabled' => TRUE,
  );

  $form['card_cvc'] = array(
    '#type' => 'textfield',
    '#title' => t('CVC:'),
    '#size' => 4,
    '#attributes' => array(
      'autocomplete' => 'off',
      'class' => array('card-cvc'),
    ),
    '#disabled' => TRUE,
  );

  $form['card_expiry_month'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration Month'),
    '#size' => 2,
    '#attributes' => array(
      'class' => array('card-expiry-month'),
    ),
  );

  $form['card_expiry_year'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration Year'),
    '#size' => 4,
    '#attributes' => array(
      'class' => array('card-expiry-year'),
    ),
  );

  $form['stripe_token'] = array(
    '#type' => 'hidden',
    '#title' => t('Amount:'),
  );

  // Retrieve the active API Key.
  //TODO abstract this mess.
  $status = variable_get('stripe_key_status', 'test');
  $pub_key_name = 'stripe_' . $status . '_publishable';
  $pub_key = variable_get($pub_key_name, '');
  
  if ($pub_key !== '') {
    // Add the submit button. Can't submit without a key.
    $form['card_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit Payment'),
      '#attributes' => array(
        'class' => array('submit-button'),
      ),
    );

    // Attach the javascript from Stripe.
    $form['#attached']['js'] = array(
      'https://js.stripe.com/v1/' => array(
        'type' => 'external',
      ),
      drupal_get_path('module', 'stripe') . '/stripe.js',
      // And set our key.
      'jQuery(document).ready(function () { Stripe.setPublishableKey("' . $pub_key . '"); });' => array(
        'type' => 'inline', 'scope' => 'footer', 'weight' => 5
      ),
    );
  }

  return $form;
}
