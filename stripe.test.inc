<?php

/**
 * @file
 * Stripe test form building callback and handlers.
 */

function stripe_admin_test($form, $form_state) {
  $form['stripe'] = array(
    '#type' => 'stripe_payment',
    '#address' => isset($form_state['values']['toggle_address']) ? (boolean) $form_state['values']['toggle_address'] : FALSE,
    '#prefix' => '<div id="stripe-wrapper">',
    '#suffix' => '</div>',
    '#weight' => 10,
  );
  $form['toggle_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ask for address'),
    '#ajax' => array(
      'callback' => 'stripe_admin_test_ajax',
      'wrapper' => 'stripe-wrapper',
    ),
    '#default_value' => (boolean) $form['stripe']['#address'],
  );
  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount:'),
    '#description' => '<em>cents</em>',
    '#default_value' => 99,
    '#size' => 6,
  );
  $form['card_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Payment'),
    '#attributes' => array(
      'class' => array('submit-button'),
    ),
  );
  return $form;
}

function stripe_admin_test_ajax($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['stripe'];
}

function stripe_admin_test_submit($form, &$form_state) {
  if (($library = libraries_load('stripe')) && !empty($library['loaded'])) {
    try {
      $charge = Stripe_Charge::create(array(
          "amount" => $form_state['values']['amount'], // amount in cents, again
          "currency" => "usd",
          "card" => $form_state['values']['stripe_token'],
          "description" => "Test Charge from " . variable_get('site_name', 'My Drupal Site'))
      );
      drupal_set_message("Success! Card was successfully charged for the amount of " . check_plain($form_state['values']['amount']));
    }
    catch (Exception $e) {
      form_set_error('', $e->getMessage());
      $form_state['rebuild'] = TRUE;
      return;
    }
  }
}
