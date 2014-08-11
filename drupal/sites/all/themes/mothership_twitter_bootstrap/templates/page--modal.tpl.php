<?php
//kpr(get_defined_vars());
//kpr($theme_hook_suggestions);
//template naming
//page--[CONTENT TYPE].tpl.php
?>
<?php if( theme_get_setting('mothership_poorthemers_helper') ){ ?>
<!--page--modal.tpl.php-->
<?php } ?>

<?php print $mothership_poorthemers_helper; ?>

<div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
  <h3><?php print $title; ?></h3>
</div>
<div class="modal-body">
    <?php if($messages): ?>
        <div class="drupal-messages">
          <?php print $messages; ?>
        </div>
    <?php endif; ?>
<?php print render($page['content_pre']); ?>

<?php print render($page['content']); ?>

<?php print render($page['content_post']); ?>

</div>
