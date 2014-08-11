<?php

/**
 * @file
 * community_tags.ajax.inc
 *
 * Handles the server side Ajax interactions of Community Tags.
 *
 * @defgroup community_tags_ajax Community Tags server side Ajax interactions.
 * @{
 */


/**
 * Callback for the JS tagger.
 */
function community_tags_from_js($entity_type, $entity_id, $vid = NULL) {
  global $user;

  if ($entity_id && isset($_POST['token']) && drupal_valid_token($_POST['token'], 'community_tags_form')) {
    $tags = (isset($_POST['tags']) && is_array($_POST['tags'])) ? $_POST['tags'] : array();

    // Merge in new tag and save.
    $tags = array_unique(array_merge($tags, drupal_explode_tags($_POST['add'])));

    //There should always be $vid, but if not, this will make a best guess.
    if (!$vid) {
      $vid = array_shift(community_tags_vids_for_entity($entity_type, $entity_id));
    }

    community_tags_taxonomy_entity_save($entity_type, $entity_id, array('tags' => array($vid => $tags)), FALSE, $user->uid);

    // Fetch updated list.
    $tags = community_tags_flatten(community_tags_get_user_node_tags($user->uid, $entity_type, $entity_id, $vid));

    // Output JSON.
    print drupal_json_output(array('status' => TRUE, 'tags' => $tags, 'sequence' => $_POST['sequence']));
  }
}

/**
 * @}
 */
