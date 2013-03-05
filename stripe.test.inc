<?php

/**
 * @file
 * Stripe test form building callback and handlers.
 */

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

function stripe_admin_test_validate($form, &$form_state) {
  stripe_initialize();
  $charge = NULL;

  try {
    $charge = Stripe_Charge::create(array(
      "amount" => $form_state['values']['amount'], // amount in cents, again
      "currency" => "usd",
      "card" => $form_state['values']['stripe_token'],
      "description" => "Test Charge from " . variable_get('site_name', 'My Drupal Site'))
    );
  }
  catch (Exception $e) {
    form_set_error('', $e->getMessage());
    return;
  }

  $form_state['storage']['charge'] = $charge;
}


function stripe_admin_test_submit($form, &$form_state) {
  drupal_set_message("Success! Card was successfully charged for the amount of " . check_plain($form_state['values']['amount']));
}
