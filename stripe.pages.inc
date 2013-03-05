<?php
/**
 * @file
 * Page callbacks for the stripe module.
 */

/**
 * Handler for incoming Stripe Webhook requests.
 */
function stripe_webhooks_callback() {
  stripe_initialize();
  // Read in and decode the event data.
  $body = @file_get_contents('php://input');
  $event = json_decode($body);
  // For additional security - retrieve event data from ID
  $event = Stripe_Event::retrieve($event->id);

  // Invoke hook_stripe_webhook().
  module_invoke_all('stripe_webhook', $event);

  // Support for logging webhooks. (Setting be implemented in the admin UI)
  if (variable_get('stripe_log_webhooks', 0) == 1) {
    watchdog('stripe', 'Stripe Webhook ' . $event->type . ': <pre>' . print_r($event, TRUE) . '</pre>');
  }
}