<?php

/**
 * Search API data alteration callback that filters out items based on their
 * bundle.
 */
class SearchApiAlterSmgAvisEmptyContent extends SearchApiAbstractAlterCallback {

  public function supportsIndex(SearchApiIndex $index) {
    return true; //($info = entity_get_info($index->item_type)) && self::hasBundles($info);
  }

  public function alterItems(array &$items) {
    foreach ($items as $id => $item) {
      if ($item->type == 'smg_avis_subitem'
        && (!isset($item->field_smg_avis_subitem_value['und'][0]['value']) || $item->field_smg_avis_subitem_value['und'][0]['value'] == '') // avis parlement et citoyens
        && ((!isset($item->title) || $item->title == '') && (!isset($item->field_smg_avis_type['und'][0]['value']) || $item->field_smg_avis_type['und'][0]['value'] == '')) // avis candidat et citoyens
      ) {
        unset($items[$id]);
      }
    }
  }

  public function configurationForm() {
    $info = entity_get_info($this->index->item_type);
    if (self::hasBundles($info)) {
      $options = array();
      foreach ($info['bundles'] as $bundle => $bundle_info) {
        $options[$bundle] = isset($bundle_info['label']) ? $bundle_info['label'] : $bundle;
      }
      $form = array(
        'default' => array(
          '#type' => 'radios',
          '#title' => t('Which items should be filtered without empty content?'),
          '#default_value' => isset($this->options['default']) ? $this->options['default'] : 1,
          '#options' => array(
            1 => t('All but those from one of the selected bundles'),
            0 => t('Only those from the selected bundles'),
          ),
        ),
        'bundles' => array(
          '#type' => 'select',
          '#title' => t('Bundles'),
          '#default_value' => isset($this->options['bundles']) ? $this->options['bundles'] : array(),
          '#options' => $options,
          '#size' => min(4, count($options)),
          '#multiple' => TRUE,
        ),
      );
    }
    else {
      $form = array(
        'forbidden' => array(
          '#markup' => '<p>' . t("Items indexed by this index don't have bundles and therefore cannot be filtered here.") . '</p>',
        ),
      );
    }
    return $form;
  }

  /**
   * Helper method for figuring out if the entities with the given entity info
   * can be filtered by bundle.
   */
  protected static function hasBundles($entity) {
    return !empty($entity_info['entity keys']['bundle']) && !empty($entity_info['bundles']);
  }

}

