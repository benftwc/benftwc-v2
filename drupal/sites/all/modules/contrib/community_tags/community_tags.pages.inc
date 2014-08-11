<?php

/**
 * @file
 * community_tags.pages.inc
 *
 * Page handlers of Community Tags.
 *
 * @defgroup community_tags_pages Community Tags page handlers.
 * @{
 */



/**
 * Page callback: Outputs JSON for taxonomy autocomplete suggestions.
 *
 * Path: taxonomy/autocomplete
 *
 * This callback outputs term name suggestions in response to Ajax requests
 * made by the taxonomy autocomplete widget for taxonomy term reference
 * fields. The output is a JSON object of plain-text term suggestions, keyed by
 * the user-entered value with the completed term name appended.  Term names
 * containing commas are wrapped in quotes.
 *
 * For example, suppose the user has entered the string 'red fish, blue' in the
 * field, and there are two taxonomy terms, 'blue fish' and 'blue moon'. The
 * JSON output would have the following structure:
 * @code
 *   {
 *     "red fish, blue fish": "blue fish",
 *     "red fish, blue moon": "blue moon",
 *   };
 * @endcode
 *
 * @param $field_name
 *   The name of the term reference field.
 * @param $tags_typed
 *   (optional) A comma-separated list of term names entered in the
 *   autocomplete form element. Only the last term is used for autocompletion.
 *   Defaults to '' (an empty string).
 *
 * @see taxonomy_menu()
 * @see taxonomy_field_widget_info()
 */
function  community_tags_taxonomy_autocomplete($field_name, $vocabulary = false, $tags_typed = '') {
  // If the request has a '/' in the search text, then the menu system will have
  // split it into multiple arguments, recover the intended $tags_typed.
  $args = func_get_args();
  // Shift off the $field_name argument.
  array_shift($args);
  array_shift($args);
  $tags_typed = implode('/', $args);

  // Make sure the field exists and is a taxonomy field.
  if (!($field = field_info_field($field_name)) || $field['type'] !== 'taxonomy_term_reference') {
    // Error string. The JavaScript handler will realize this is not JSON and
    // will display it as debugging information.
    print t('Taxonomy field @field_name not found.', array('@field_name' => $field_name));
    exit;
  }

  // The user enters a comma-separated list of tags. We only autocomplete the last tag.
  $tags_typed = drupal_explode_tags($tags_typed);
  $tag_last = drupal_strtolower(array_pop($tags_typed));

  $matches = array();
  if ($tag_last != '') {

    // Part of the criteria for the query come from the field's own settings.
    $vids = array();
    $vocabularies = taxonomy_vocabulary_get_names();
    foreach ($field['settings']['allowed_values'] as $tree) {
      $vids[] = $vocabularies[$tree['vocabulary']]->vid;
    }
    if ($field_name == 'field_smg_moderation_tagging') {
      $vids = array($vocabulary);
    }

    $query = db_select('taxonomy_term_data', 't');
    $query->addTag('translatable');
    $query->addTag('term_access');

    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('t.name', $tags_typed, 'NOT IN');
    }
    // Select rows that match by term name.
    $tags_return = $query
      ->fields('t', array('tid', 'name'))
      ->condition('t.vid', $vids)
      ->condition('t.name', '%' . db_like($tag_last) . '%', 'LIKE')
      ->range(0, 10)
      ->execute()
      ->fetchAllKeyed();

    $prefix = count($tags_typed) ? drupal_implode_tags($tags_typed) . ', ' : '';

    $term_matches = array();
    foreach ($tags_return as $tid => $name) {
      $n = $name;
      // Term names containing commas or quotes must be wrapped in quotes.
      if (strpos($name, ',') !== FALSE || strpos($name, '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $name) . '"';
      }
      $term_matches[$prefix . $n] = check_plain($name);
    }
  }

  drupal_json_output($term_matches);
}

/**
 * Quick tag form.
 */
function community_tags_form($form, &$form_state, $edit, $title = NULL) {
  $vocabulary = taxonomy_vocabulary_load($edit['vid']);

  if ($edit['cloud']) {
    if($edit['multiple'] > 1) {
      // displaying more than 1 form so need to differentiate in headings
      $all_title = t('All !name tags', array('!name' => $vocabulary->name));
    }
    else {
      $all_title = t('All tags');
    }

    $form['cloud'] = array(
      '#type' => 'item',
      '#title' => $all_title,
      '#markup' => $edit['cloud'],
    );
  }

  $access = user_access('tag content');

  $field = $edit['field'];

  if($edit['multiple'] > 1) {
    // displaying more than 1 form so need to differentiate in headings
    $my_title = t('My !name tags', array('!name' => $vocabulary->name));
  }
  else {
    $my_title = t('My tags');
  }

  $form['tags'] = array(
    '#type' => 'textfield',
    '#title' => $my_title,
    '#maxlength' => 100,
    '#default_value' => $edit['tags'],
    '#required' => FALSE,
    '#autocomplete_path' => 'community_tags/taxonomy/autocomplete/' . $field['field_name'] . '/' . $edit['vid'],
    '#attributes' => array('class' => array('form-tags form-tags-' . $edit['vid'])),
    '#access' => $access,
  );
  if ($edit['inline']) {
    $form['tags']['#size'] = 20;
  }

  if (!$access) {
    $destination = drupal_get_destination();
    $form['login'] = array(
      '#type' => 'markup',
      '#value' => '<div>' . t('<a href="@login">Login</a> or <a href="@register">register</a> to tag items', array('@login' => url('user/login', array('query' => $destination)), '@register' => url('user/register', array('query' => $destination)))) . '</div>',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => $access,
  );

  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $edit['entity'],
  );

  $form['entity_id'] = array(
    '#type' => 'hidden',
    '#title' => t('NID'),
    '#value' => $edit['entity_id'],
  );

  $form['entity_type'] = array(
    '#type' => 'hidden',
    '#title' => t('entity_type'),
    '#value' => $edit['entity_type'],
  );

  $form['vid'] = array(
    '#type' => 'hidden',
    '#default_value' => $edit['vid'],
  );

  $entity_id = $edit['entity_id'];
  $vid = $edit['vid'];
  $names = $edit['names'];

  $form['#attached']['js'] = array(
    // Add a JavaScript setting. Note that when the key is a number, the 'data' property will be used.
    array(
      'data' => array('communityTags' => array('e_' . $edit['entity_type'] . '_' . $entity_id => array('v_' . $vid => array('tags' => $names, 'url' => url('community-tags/js/' . $edit['entity_type'] . '/' . $entity_id . '/' . $vid), 'add' => t('Add'), 'token' => drupal_get_token('community_tags_form'))))),
      'type' => 'setting'
    ),
  );

  return $form;
  // drupal_add_js(array('communityTags' => array('n_'. $node->nid => array('v_'. $vid => array('tags' => $names, 'url' => url('community-tags/js/'. $node->nid .'/'. $vid), 'add' => t('Add'), 'token' => drupal_get_token('community_tags_form'))))), 'setting');

}

/**
 * Submit callback for quick tag form.
 */
function community_tags_form_submit($form, &$form_state) {
  global $user;
  community_tags_taxonomy_entity_save($form_state['values']['entity'], array('tags' => array($form_state['values']['vid'] => $form_state['values']['tags'])), FALSE, $user->uid);

  // $form_state['redirect'] = 'node/' . $form_state['values']['nid'];
}

/**
 * Theme the quick tag form.
 * @ingroup themeable
 */
function theme_community_tags_form($variables) {
  $form = $variables['form'];
  $output = '';

  // if(isset($form['cloud'])) {
  //   $output .= theme('form_element', array('#title' => $form['cloud']['#title']), drupal_render($form['cloud']));
  // }

  $output .= drupal_render_children($form);

  // We add the JS file this late, to ensure it comes after autocomplete.js.
  drupal_add_css(drupal_get_path('module', 'community_tags') . '/community_tags.css');
  drupal_add_js(drupal_get_path('module', 'community_tags') . '/community_tags.js');

  return $output;
}

/**
 * Menu callback:
 */
function community_tags_mypage($uid = NULL) {
  global $user;
  $uid = isset($uid) ? $uid : $user->uid;

  $tags_query = _community_tags_get_tag_result('user', 1000, $uid);
  $tags = $tags_query->fetchAllAssoc('tid');

  $links = array();

  foreach($tags as $tid => $tag) {
    $links[$tid]['title'] = $tag->name . ' (count: ' . $tag->count . ')';
    // TODO nid not hardocded
    $links[$tid]['href'] = url('node/67/avis', array('absolute' => true, 'query' => array('f[0]' => 'smg_community_tags:' . $tag->tid)));
    $links[$tid]['attributes'] = array('style' => '');
  }
  sort($links);

  $return = '<style type="text/css">.region-content ul.links li {float: none} .region-content ul.links li a {float: none}</style>';
  $return .= theme('links', array('links' => $links));

  return $return;
}

