<?php
// Overview Settings
function circ_itemtypes_overview_form() {
  $types = circ_get_item_types();
  foreach ($types as $type) {
    $rows[] = array(
      'icon' => _circ_item_type_to_icon($type),
      'code' => $type['code'],
      'display' => $type['display'],
      'configure' => l('configure', 'admin/library/circ/itemtypes/edit/' . $type['code']),
      'delete' => l('delete', 'admin/library/circ/itemtypes/delete/' . $type['code'] . '/delete'),
    );
  }

  $form = array();
  $form[] = array(
    '#type' => 'markup',
    '#value' => theme('table', array('Icon', 'Item Code', 'Item Display', array('data' => 'Operations', 'colspan' => 2)), $rows),
  );

  return $form;
}
function circ_itemtypes_settings_form() {
  $form = array();
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['icon'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default Icon'),
  );

  $thumbnail = variable_get('circ_default_item_type_icon', '');
  $form['icon'][] = array(
    '#type' => 'markup',
    '#value' => (empty($thumbnail)) ? '' : '<div>Full Size Image:<br>' . theme('image', $thumbnail, 'default icon', 'default icon') . '</div>',
  );
  $form['icon'][] = array(
    '#type' => 'markup',
    '#value' => (empty($thumbnail)) ? '' : '<div>36 x 36 Thumbnail:<br>' . theme('image', $thumbnail, 'default icon', 'default icon', array('style' => 'max-width:36px;max-height:36px')) . '</div>',
  );
  $form['icon'][] = array(
    '#type' => 'markup',
    '#value' => (empty($thumbnail)) ? '' : '<div>18px x 18px Thumbnail:<br>' . theme('image', $thumbnail, 'default icon', 'default icon', array('style' => 'max-width:18px;max-height:18px')) . '</div>',
  );
  $form['icon']['icon_upload'] = array(
    '#type' => 'file',
    '#title' => t('Default icon'),
  );
  $form['icon']['icon_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Default icon path'),
    '#default_value' => variable_get('circ_default_item_type_icon', ''),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

// Settings Validation
function circ_itemtypes_settings_form_validate($form_id, $form_values) {
}

// Settings Submit
function circ_itemtypes_settings_form_submit($form_id, $form_values) {
  $icon_path = $form_values['icon_path'];

  $dst = file_directory_path();
  // Check for a new uploaded favicon, and use that instead.
  if ($file = file_check_upload('icon_upload')) {
    $parts = pathinfo($file->filename);
    $filename = 'icon_default.'. $parts['extension'];

    if ($file = file_save_upload('icon_upload', $filename, 1)) {
      $icon_path = $dst . '/' . $filename;
    }
  }

  variable_set('circ_default_item_type_icon', $icon_path);
  drupal_set_message('Default item type icon added/saved.');
}

/**
 * Adds item type
 *
 */
function circ_add_item_type_form($item_type = null) {
  if (!empty($item_type)) {
    $item_type = circ_get_item_type($item_type);
  }

  $form =array();
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['item_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Item Type Settings'),
  );
  $form['item_type']['code'] = array(
    '#type' => 'textfield',
    '#title' => t('Item Code'),
    '#default_value' => $item_type['code'],
    '#disabled' => (empty($item_type)) ? false : true,
  );
  $form['item_type']['display'] = array(
    '#type' => 'textfield',
    '#title' => t('Item Display'),
    '#default_value' => $item_type['display'],
  );

  $form['icon'] = array(
    '#type' => 'fieldset',
    '#title' => t('Item Type Icon Settings'),
  );
  $form['icon']['icon_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Custom Icon'),
    '#default_value' => $item_type['icon'],
  );
  $form['icon']['icon_upload'] = array(
    '#type' => 'file',
    '#title' => t('Item Icon'),
  );
  $form[] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

// Add validate
function circ_add_item_type_form_validate($form_id, $form_values) {
  if ($icon = file_check_upload('icon_upload')) {
    if (preg_match('/(.+)\.([^\.]+)$/', $icon->filename, $match)) {
      $icon->filename = 'icon_' . $form_values['code'] . '.' . strtolower($match[2]);
    }
  }
}

// Add submit
function circ_add_item_type_form_submit($form_id, $form_values) {
  $code = $form_values['code'];
  $display = $form_values['display'];
  $icon_path = $form_values['icon_path'];

  $dst = file_directory_path();
  if ($icon = file_save_upload('icon_upload', $dst, TRUE)) {
    $icon_path = $icon->filepath;
  }

  circ_add_item_type($code, $display, $icon_path);
  drupal_set_message('Item type added/saved.');
  drupal_goto('admin/library/circ/itemtypes');
}

/**
 * Confirms deletion of an item type
 */
function circ_item_type_confirm_delete($type) {
  $form['code'] = array(
    '#type' => 'hidden',
    '#value' => $type,
  );
  return confirm_form($form,
                        t('Are you sure you want to delete the item type !type?', array('!type' => $type)),
                        'admin/library/circ',
                        NULL,
                        t('Delete'),
                        t('Cancel'));
}

/**
 * Deletes an item type
 */
function circ_item_type_confirm_delete_submit($form_id, $form_values) {
  circ_del_item_type($form_values['code']);
  drupal_set_message('Item type deleted.');
  return 'admin/library/circ/itemtypes';
}
