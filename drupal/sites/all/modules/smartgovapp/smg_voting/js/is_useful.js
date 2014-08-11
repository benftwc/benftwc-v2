/**
 *
 */
(function($){
  if (Drupal.ajax) {
    Drupal.ajax.prototype.commands.isusefulUpdate = function (ajax, response, status) {
      response.selector = $('.is-useful', ajax.element.form);
      ajax.commands.insert(ajax, response, status);
    };
  }
})(jQuery);

(function($){

  Drupal.behaviors.isuseful = {
    attach: function (context) {
      $('.field-type-is-useful .form-type-select').once('is_useful', function() {
        var $this = $(this);
        var $container = $('<div class="is-useful-widget rate-widget clearfix"></div>');
        var $select = $('select', $this);

        // Setup the Yes/No/Maybe buttons
        var $options = $('option', $this).not('[value="-"], [value="0"], [value="maybe"]');
        var index = -1;
        
        // yes
        var yes_count = parseInt($options[0].text.replace(/\(([0-9]+)\)(.*)/ig, '$1'));
        var no_count = parseInt($options[1].text.replace(/\(([0-9]+)\)(.*)/ig, '$1'));
        var rate_class = 'rate-button';
        if ($options[0].value == $select.val()) {
          rate_class = "rate-up rate-button";
        }
        else if ($options[1].value == $select.val()) {
          rate_class = "rate-down rate-button";
        }
        // var percent_down = yes_count + no_count > 0 ? Math.round((no_count * 100) / (yes_count + no_count)) : 0;
        // var percent_up = yes_count + no_count > 0 ? Math.round((yes_count * 100) / (yes_count + no_count)) : 0;

        var up_txt   = '<a class="btn btn-success rate-votes-btn rate-votes-btn-up" rel="nofollow" href="#yes" title="D\'accord"><i class="icon-thumbs-up"></i>D\'accord</a><span class="rate-votes-num rate-votes-num-up">' + yes_count + '</span>';
        var down_txt = '<a class="btn btn-danger rate-votes-btn rate-votes-btn-down" rel="nofollow" href="#no" title="Pas d\'accord"><i class="icon-thumbs-down"></i>Pas d\'accord</a><span class="rate-votes-num rate-votes-num-down">' + no_count + '</span>';
// $('<table class="' + rate_class + '"><tbody><tr><td><a class="rate-button rate-votes-btn rate-votes-btn-down" rel="nofollow" href="#no" title="Contre">Contre</a></td><td class="rate-votes-num rate-votes-num-down">' + no_count + '</td><td class="rate-votes-bar-container"><div class="rate-votes-bar"><div class="rate-votes-bar-down" style="width:' + percent_down + '%;"></div><div class="rate-votes-bar-up" style="width:' + percent_up + '%;"></div></div></td><td class="rate-votes-num rate-votes-num-up">' + yes_count + '</td><td><a class="rate-button rate-votes-btn rate-votes-btn-up" rel="nofollow" href="#yes" title="Pour">Pour</a></td></tr></tbody></table>')
        $('<div class="' + rate_class + '' + '">' + up_txt + down_txt + '</span></div>').appendTo($container);

        /*
        $options.each(function(i, element) {
          var classes = 'is-useful-' + element.value;
          var text = element.text.replace(/\([0-9]*\) /ig, '');
          var count = element.text.replace(text, '');
          count = count.replace(/\(([0-9]*)\)/ig, '$1');
          if (element.value == $select.val()) {
            classes += ' selected';
          }
          $('<div class="is-useful-link"><span class="count">'+ count +'</span><a href="#' + element.value + '" title="' + text + '">' + text + '</a></div>')
            .addClass(classes)
            .appendTo($container);
        });
        */
        $container.find('a')
          .bind('click', $this, Drupal.behaviors.isuseful.vote);
        // Attach the new widget and hide the existing widget.
        $select.after($container).css('display', 'none');
      });
        // Hide all submit button
        $('.is-useful-form .form-submit').hide();

    },
    vote: function(event) {
      var $this = $(this);
      var $widget = event.data;
      var value = this.hash.replace('#', '');
      $('select', $widget).val(value).change();
      event.preventDefault();
    }
  };
})(jQuery);
