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
  if (is_object($event) && isset($event->id)) {
    $event = Stripe_Event::retrieve($event->id);
    if (is_object($event) && isset($event->id)) {
      module_invoke_all('stripe_webhook', $event);
      if (variable_get('stripe_log_webhooks', 0) == 1) {
        watchdog('stripe', 'Stripe Webhook ' . $event->type . ': <pre>' . print_r($event, TRUE) . '</pre>');
      }
      return 'OK';
    }
  }
  // Something went wrong if we got here so return 404. If it is in fact
  // coming from Stripe, they will attempt to resend it for 3 days.
  header('HTTP/1.0 404 Not Found');
}
