(function ($) {

Drupal.behaviors.stripeFormFix = {
  attach: function (context, settings)  {
    // This function must stay in here since we need context.
    stripeFormBindHandler = function (index, value) {
      // The identifier is a class on the card field.
      $('.stripe-number.' + value, context)
        .closest('form')
        .submit(stripeSubmitHandler);
    };

    // Enable the form elements and clean up any classes.
    $('.stripe-number, .stripe-cvc', context)
      .removeAttr('disabled')
      .parent()
      .removeClass('form-disabled');

    // Set the publishable key.
    Stripe.setPublishableKey(settings.stripe.pubKey);

    // Now bind the submit handler to each form.
    $.each(settings.stripe.identifiers, stripeFormBindHandler);
  }
};

/**
 * Submit a form to Stripe to create a token. Clears out all errors.
 */
stripeSubmitHandler = function (eventObject) {
  // Add a submitted class so we can find this item in the response handler.
  $(this).addClass('stripe-submitted');

  // Clear out all errors.
  $('.error', this).removeClass('error');
  $('.stripe-errors', this).remove();

  // Create the token.
  Stripe.createToken({
      number: $('.stripe-number', this).val(),
      cvc: $('.stripe-cvc', this).val(),
      exp_month: $('.stripe-exp_month', this).val(),
      exp_year: $('.stripe-exp_year', this).val()
  }, stripeResponseHandler);

  // Prevent the form from submitting with the default action.
  return false;
};

/**
 * Handle a Stripe response, displaying errors if necessary. Otherwise, this
 * will submit the form to Drupal.
 */
stripeResponseHandler = function(status, response) {
  console.log(response);
  var form$ = $('.stripe-submitted');
  if (response.error) {
    //show the errors on the form.
    form$.prepend('<div class="stripe-errors messages error"></div>');
    $('.stripe-' + response.error.param, form$).addClass('error');
    $('.stripe-errors', form$).append(Drupal.t(response.error.message));

    // Clean up the submitted class.
    form$.removeClass('stripe-submitted');
  }
  else {
    // token contains id, last4, and card type
    var token = response.id;
    // insert the token into the form so it gets submitted to the server
    $('input[name=stripe_token]').val(token);
    // and submit
    form$.get(0).submit();
  }
};

})(jQuery);