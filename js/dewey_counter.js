(function ($) {
  $(document).ready(function() {
    $.ajax({
      type: "POST",
      cache: false,
      url: Drupal.settings.dewey.url,
      data: Drupal.settings.dewey.data
    });
  });
})(jQuery);
