<?php

/**
 * @file
 * Stripe test form building callback and handlers.
 */

function stripe_admin_test() {
  if (!stripe_get_key("secret") || !stripe_get_key("publishable")) {
    $form['error_markup'] = array(
      '#markup' => t("Please configure your Stripe API keys before attempting any test charges"),
    );
    return $form;
  }

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

/**
 * Implements hook_submit().
 */
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
