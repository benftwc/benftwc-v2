<?php

/**
 * Implements hook_form_alter().
 */
function mothership_twitter_bootstrap_form_alter(&$form, &$form_state, $form_id) {
  // Id's of forms that should be ignored
  // Make this configurable?
  $form_ids = array(
    'node_form',
    'system_site_information_settings',
    'user_profile_form',
    'node_delete_confirm',
  );

  // Only wrap in container for certain form
  if (isset($form['#form_id']) && !in_array($form['#form_id'], $form_ids) && !isset($form['#node_edit_form'])) {
    $form['actions']['#theme_wrappers'] = array();
  }
}  

/**
 * Implements hook_form_FORM_ID_alter() for search_form().
 */
function mothership_twitter_bootstrap_form_search_form_alter(&$form, &$form_state) {
  $form['#attributes']['class'][] = 'form-search';
  $form['#attributes']['class'][] = 'pull-left';

  $form['basic']['keys']['#title'] = '';
  $form['basic']['keys']['#attributes']['class'][] = 'search-query';
  $form['basic']['keys']['#attributes']['class'][] = 'span2';
  $form['basic']['keys']['#attributes']['placeholder'] = t('Search');

  // Hide the default button from display and implement a theme wrapper to add
  // a submit button containing a search icon directly after the input element.
  $form['basic']['submit']['#attributes']['class'][] = 'element-invisible';
  $form['basic']['keys']['#theme_wrappers'][] = 'bootstrap_search_form_wrapper';

  // Apply a clearfix so the results don't overflow onto the form.
  $form['#suffix'] = '<div class="clearfix"></div>';
  $form['#attributes']['class'][] = 'content-search';
}

/**
 * Implements hook_form_FORM_ID_alter() for search_block_form().
 */
function mothership_twitter_bootstrap_form_search_block_form_alter(&$form, &$form_state) {
  $form['#attributes']['class'][] = 'form-search';
  $form['#attributes']['class'][] = 'pull-left';

  $form['search_block_form']['#title'] = '';
  $form['search_block_form']['#attributes']['class'][] = 'search-query';
  $form['search_block_form']['#attributes']['class'][] = 'span2';
  $form['search_block_form']['#attributes']['placeholder'] = t('Search');

  // Hide the default button from display and implement a theme wrapper to add
  // a submit button containing a search icon directly after the input element.
  $form['actions']['submit']['#attributes']['class'][] = 'element-invisible';
  $form['search_block_form']['#theme_wrappers'][] = 'bootstrap_search_form_wrapper';

  // Apply a clearfix so the results don't overflow onto the form.
  $form['#attributes']['class'][] = 'content-search';
}

/**
 * Returns HTML for a form element.
 */
function mothership_twitter_bootstrap_form_element(&$variables) {
  $element = &$variables['element'];
  // This is also used in the installer, pre-database setup.
  $t = get_t();

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
  );

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }

  $exclude_control = FALSE;
  $control_wrapper = '<div class="controls">';
  // Add bootstrap class
  if (isset($element['#type']) && ($element['#type'] == "radio" || $element['#type'] == "checkbox")){
    $exclude_control = TRUE;
  }
  else {
    $attributes['class'] = array('control-group');
  }

  // Check for errors and set correct error class
  if (isset($element['#parents']) && form_get_error($element)) {
    $attributes['class'][] = 'error';
  }

  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  $attributes['class'][] = 'form-item';
  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';

  // Prepare input whitelist - added to ensure ajax functions don't break
  $whitelist = _mothership_twitter_bootstrap_element_whitelist();

  switch ($element['#title_display']) {
    case 'before':
    case 'invisible':
      $output .= ' ' . theme('form_element_label', $variables);
        if ( !empty($element['#description']) ) {
            $output .= '<p class="help-block">' . $element['#description'] . "</p>\n";
        }
      // Check if item exists in element whitelist
      if (isset($element['#id']) && in_array($element['#id'], $whitelist)) {
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
        $exclude_control = TRUE;
      }
      else {
        $output = $exclude_control ? $output : $output.$control_wrapper;
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      }
      break;

    case 'after':
      $output = $exclude_control ? $output : $output.$control_wrapper;
      $variables['#children'] = ' ' . $prefix . $element['#children'] . $suffix;
      $output .= ' ' . theme('form_element_label', $variables) . "\n";
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output = $exclude_control ? $output : $output.$control_wrapper;
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;
  }


  // Check if control wrapper was added to ensure we close div correctly
  if ($exclude_control) {
    $output .= "</div>\n";
  }
  else {
    $output .= "</div></div>\n";
  }
  return $output;
}

/**
 * Returns HTML for a form element label and required marker.
 */
function mothership_twitter_bootstrap_form_element_label(&$variables) {
  $element = $variables['element'];
  // This is also used in the installer, pre-database setup.
  $t = get_t();

  // If title and required marker are both empty, output no label.
  if ((!isset($element['#title']) || $element['#title'] === '') && empty($element['#required'])) {
    return '';
  }

  // If the element is required, a required marker is appended to the label.
  $required = !empty($element['#required']) ? theme('form_required_marker', array('element' => $element)) : '';

  $title = filter_xss_admin($element['#title']);

  $attributes = array();
  // Style the label as class option to display inline with the element.
  if ($element['#title_display'] == 'after') {
    $attributes['class'][] = 'option';
    $attributes['class'][] = $element['#type'];
  }
  // Show label only to screen readers to avoid disruption in visual flows.
  elseif ($element['#title_display'] == 'invisible') {
    $attributes['class'][] = 'element-invisible';
  }

  if (!empty($element['#id'])) {
    $attributes['for'] = $element['#id'];
  }

  // @Bootstrap: Add Bootstrap control-label class except for radio.
  if ($element['#type'] != 'radio') {
    $attributes['class'][] = 'control-label';
  }
  // @Bootstrap: Insert radio and checkboxes inside label elements.
  $output = '';
  if ( isset($variables['#children']) ) {
    $output .= $variables['#children'];
  }

  // @Bootstrap: Append label
  $output .= $t('!title !required', array('!title' => $title, '!required' => $required));

  // The leading whitespace helps visually separate fields from inline labels.
  return ' <label' . drupal_attributes($attributes) . '>' . $output . "</label>\n";
}

/*
theme_textfield()
http://api.drupal.org/api/drupal/includes--form.inc/function/theme_textfield
set the size to 30 instead of 60
remove form-text class
*/
function mothership_twitter_bootstrap_text_format_wrapper($variables) {
  $element = $variables['element'];
  $output = '<div class="text-format-wrapper">';
  if (!empty($element['#description'])) {
    $output .= '<p class="help-block">' . $element['#description'] . '</p>';
  }
  $output .= $element['#children'];
  $output .= "</div>\n";

  return $output;
}

/**
 * Preprocessor for theme('button').
 */
function mothership_twitter_bootstrap_preprocess_button(&$vars) {
  $vars['element']['#attributes']['class'][] = 'btn';

  if (isset($vars['element']['#value'])) {
    $classes = array(
      //specifics
      t('Save and add') => 'btn-info',
      t('Add another item') => 'btn-info',
      t('Add effect') => 'btn-primary',
      t('Add and configure') => 'btn-primary',
      t('Update style') => 'btn-primary',
      t('Download feature') => 'btn-primary',

      //generals
      t('Log in') => 'btn-primary',
      t('Create new account') => 'btn-primary',
      t('E-mail new password') => 'btn-primary',
      t('Save') => 'btn-primary',
      t('Apply') => 'btn-primary',
      t('Create') => 'btn-primary',
      t('Confirm') => 'btn-primary',
      t('Submit') => 'btn-primary',
      t('Export') => 'btn-primary',
      t('Import') => 'btn-primary',
      t('Restore') => 'btn-primary',
      t('Rebuild') => 'btn-primary',
      t('Search') => 'btn-primary',
      t('Upload') => 'btn-primary',
      t('Add') => 'btn-info',
      t('Update') => 'btn-info',
      t('Delete') => 'btn-danger',
      t('Remove') => 'btn-danger',

      t('Modérer') => 'btn-primary',
    );
    foreach ($classes as $search => $class) {
      if (strpos($vars['element']['#value'], $search) !== FALSE) {
        $vars['element']['#attributes']['class'][] = $class;
        break;
      }
    }
  }
}

/**
 * Returns HTML for a button form element.
 */
function mothership_twitter_bootstrap_button($variables) {
  $element = $variables['element'];
  $label = $element['#value'];
  element_set_attributes($element, array('id', 'name', 'value', 'type'));

  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];
  if (!empty($element['#attributes']['disabled'])) {
    $element['#attributes']['class'][] = 'form-button-disabled';
  }

  // Prepare input whitelist - added to ensure ajax functions don't break
  $whitelist = _mothership_twitter_bootstrap_element_whitelist();

  if (in_array($element['#id'], $whitelist)) {
    return '<input' . drupal_attributes($element['#attributes']) . ">\n"; // This line break adds inherent margin between multiple buttons
  }
  else {
    return '<button' . drupal_attributes($element['#attributes']) . '>'. $label ."</button>\n"; // This line break adds inherent margin between multiple buttons
  }
}

/**
 * Returns an array containing ids of any whitelisted drupal elements
 */
function _mothership_twitter_bootstrap_element_whitelist() {
/**
 * Why whitelist an element?
 * The reason is to provide a list of elements we wish to exclude
 * from certain modifications made by the bootstrap theme which
 * break core functionality - e.g. ajax.
 */
  return array(
    'edit-refresh',
    'edit-pass-pass1',
    'edit-pass-pass2',
    'panels-ipe-cancel',
    'panels-ipe-customize-page',
    'panels-ipe-save',
  );
}
