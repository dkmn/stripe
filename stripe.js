(function ($) {

Drupal.behaviors.stripeFormFix = {
  attach: function (context)  {
    $('.card-number').removeAttr('disabled').parent().removeClass('form-disabled');
    $('.card-cvc').removeAttr('disabled').parent().removeClass('form-disabled');

    $("#stripe-admin-test").submit(function(event) {
        var amount = $('.amount').val();
        Stripe.createToken({
            number: $('.card-number').val(),
            cvc: $('.card-cvc').val(),
            exp_month: $('.card-expiry-month').val(),
            exp_year: $('.card-expiry-year').val()
        }, amount, stripeResponseHandler);
        // prevent the form from submitting with the default action
        return false;
    });
    
    stripeResponseHandler = function(status, response) {
      console.log(response);
      if (response.error) {
        //show the errors on the form
        $("#stripe-admin-test").after($('<div class="payment-errors"></div>').html(response.error.message));
      }
      else {
        var $form = $("#stripe-admin-test");
        // token contains id, last4, and card type
        var token = response['id'];
        
        var form$ = $("#stripe-admin-test");

        // insert the token into the form so it gets submitted to the server
        $('input[name=stripe_token]').val(token);
        // and submit
        form$.get(0).submit();
      }
    }
  }
}
})(jQuery);