<?php

$theme_path = drupal_get_path('theme', 'mothership_twitter_bootstrap');
require_once $theme_path . '/inc/pager.inc';
require_once $theme_path . '/inc/theme.inc';
require_once $theme_path . '/inc/menu.inc';
require_once $theme_path . '/inc/form.inc';




function mothership_twitter_bootstrap_preprocess_page(&$variables) {
    $theme_path = drupal_get_path('theme', 'mothership_twitter_bootstrap');
    drupal_add_js( $theme_path . '/twitter_bootstrap/js/bootstrap-transition.js');
    drupal_add_js( $theme_path . '/twitter_bootstrap/js/bootstrap-collapse.js');
    drupal_add_js($theme_path . '/twitter_bootstrap/js/bootstrap-modal.js');
    drupal_add_js($theme_path . '/twitter_bootstrap/js/bootstrap-dropdown.js');
    drupal_add_js($theme_path . '/twitter_bootstrap/js/bootstrap-tooltip.js');
    drupal_add_js($theme_path . '/twitter_bootstrap/js/bootstrap-popover.js');
    drupal_add_js(drupal_get_path('module', 'jquery_update') .
      '/replace/ui/ui/jquery-ui.js');
    drupal_add_js(drupal_get_path('module', 'jquery_update') .
      '/replace/ui/ui/jquery.ui.accordion.js');
    
    //Adapte main content widht depending of sidebar
    $variables['main_class'] = 'span16';
  if(!empty($variables['page']['sidebar_first']) && !empty($variables['page']['sidebar_second'])){
      $variables['main_class'] = 'span8';
  }
  elseif(!empty($variables['page']['sidebar_first']) || !empty($variables['page']['sidebar_second'])){
      $variables['main_class'] = 'span11';
  }

  // ajax modal
  if (isset($_GET['modal']) && $_GET['modal'] == true) {
    $variables['theme_hook_suggestions'][] =  'page__modal';
  }
}

function mothership_twitter_bootstrap_preprocess_html(&$vars,$hook) {
  // ajax modal
  if (isset($_GET['modal']) && $_GET['modal'] == true) {
    $vars['theme_hook_suggestions'][] =  'html__modal';
  }
}


function mothership_twitter_bootstrap_preprocess_entity(&$variables){
  // var_dump(array_keys($variables), $variables['theme_hook_suggestions']);
  $variables['theme_hook_suggestions'][] = 'entity__' . $variables['entity_type'];
  $variables['theme_hook_suggestions'][] = 'entity__' . $variables['entity_type'] . '__' . $variables[$variables['entity_type']]->type;

}


