(function ($) {
  'use strict';
  $(document).ready(function () {
    $.ajax({
      type: 'POST',
      cache: false,
      url: Drupal.settings.dewy.url,
      data: Drupal.settings.dewy.data
    });
  });
})(jQuery);
