<?php
// $Id$

/**
 * Implements hook_form_alter().
 *
 * Allows the profile to alter the site configuration form. Adds a default
 * value for site name during the configure step of site install.
 */
function smartgov_candidat_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'install_configure_form') {
    // Set default for site name field.
    $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
  }

}

/**
 * Implements hook_appstore_stores_info().
 *
 * Used to get an apps manifest from the Opish apps server.
 * Identifies current site to server for voting and other functionality.
 */
// function smartgov_candidat_apps_servers_info() {
//  $info =  drupal_parse_info_file(dirname(__file__) . '/smartgov_candidat.info');
//  return array(
//    'smartgov_candidat' => array(
//      'title' => 'smartgov_candidat',
//      'description' => "Apps for the smartgov_candidat distribution",
//      'manifest' => variable_get('smartgov_candidat_apps_server_url', 'http://appserver.smartgov_candidatapp.com/app/query/smartgov_candidat'),
//      'profile' => 'smartgov_candidat',
//      'profile_version' => isset($info['version']) ? $info['version'] : '7.x-1.0-alpha2',
//      'server_name' => isset($GLOBALS['base_url']) ? $GLOBALS['base_url'] : '',
//      'server_ip' => isset($_SERVER['SERVER_ADDR']) ? $_SERVER['SERVER_ADDR'] : '',
//    ),
//  );
// }
// 
/**
 * Implements hook_init()
 *
 * Adds data to the js settings array about the profile.
 */
function smartgov_candidat_init() {
 $cache = cache_get("smartgov_candidat_info");
 if (isset($cache->data)) {
   $data = $cache->data;
 }
 else {
   $info =  drupal_parse_info_file(dirname(__file__) . '/smartgov_candidat.info');
   $version = array_key_exists('version', $info) ? $info['version'] : '7.x-1.x';
   $data = array("profile" => "smartgov_candidat", "profile_version" => $version);
   cache_set("smartgov_candidat_info", $data);
 }
 drupal_add_js($data, 'setting');
}



/**
 * Implement hook_install_tasks().
 */
function smartgov_candidat_install_tasks($install_state) {
  // Determine whether translation import tasks will need to be performed.
  $needs_translations = count($install_state['locales']) > 1 && !empty($install_state['parameters']['locale']) && $install_state['parameters']['locale'] != 'en';

  return array(
    'smartgov_candidat_import_translation' => array(
      'display_name' => st('Set up translations'),
      'display' => $needs_translations,
      'run' => $needs_translations ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
      'type' => 'batch',
    ),
  );
}

/**
 * Implement hook_install_tasks_alter().
 *
 * Perform actions to set up the site for this profile.
 */
function smartgov_candidat_install_tasks_alter(&$tasks, $install_state) {
  // Remove core steps for translation imports.
  unset($tasks['install_import_locales']);
  unset($tasks['install_import_locales_remaining']);
}

/**
 * Installation step callback.
 *
 * @param $install_state
 *   An array of information about the current installation state.
 */
function smartgov_candidat_import_translation(&$install_state) {
  // Enable installation language as default site language.
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $install_locale = $install_state['parameters']['locale'];
  locale_add_language($install_locale, NULL, NULL, NULL, '', NULL, 1, TRUE);

  // Build batch with l10n_update module.
  $history = l10n_update_get_history();
  module_load_include('check.inc', 'l10n_update');
  $available = l10n_update_available_releases();
  $updates = l10n_update_build_updates($history, $available);

  module_load_include('batch.inc', 'l10n_update');
  $updates = _l10n_update_prepare_updates($updates, NULL, array());
  $batch = l10n_update_batch_multiple($updates, LOCALE_IMPORT_KEEP);
  return $batch;
}
