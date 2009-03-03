<?php

// $Id$

/**
 * @file
 * This file contains the admin functions for the Media module.
 */


/**
 * Provides the per content type settings.
 *
 * @return unknown
 */
function media_admin_settings($form = array(), $type_name = null) {
  // Default page - list global and content type sub-menus.
  if (empty($type_name)) {
    $items = array(
      l('Global and Default Media Settings', 'admin/content/media/global'),
      l('Content Type Settings', 'admin/content/media/types'),
    );
    return theme('item_list', $items, t('Configure your site\'s Media settings.'));
  }
  // Base content type page - list mode.
  if ($type_name == 'types') {
    // get all the content types
    $types = node_get_types();
    $content_types = array();
    if ($types) {
      foreach ($types as $content_type) {
        $content_types[] = l($content_type->name, 'admin/content/media/types/'. $content_type->type);
      }
    }
    return theme('item_list', $content_types, t('Configure Media settings that are specific to a content type'));
  }
  // Remaining options (global and specific type) return a form
  $form = array();
  // Global settings are currently just defaults for content types
  if ($type_name == 'global') {
    $form['media_global_enabled'] = array(
      '#title' => t('Media resource browser'),
      '#type' => 'checkbox',
      '#description' => t('Enable or Disable the Media resource browser for all types, unless specifically set for a given type.'),
      '#default_value' => variable_get('media_global_enabled', 1),
    );
    return system_settings_form($form);
  }
  // Specific content type settings.
  // Do we have cck installed?
  if (module_exists('content')) {
    $type = content_types($type_name);
  }
  else {
    // Get the specific content type.
    $type = (array) node_get_types('type', $type_name);
  }
  
  // Master settings.
  $form['media_'. $type_name .'_override'] = array(
    '#title' => t($type["name"] . ' overrides default values'),
    '#type' => 'checkbox',
    '#description' => t('Override the default settings for this content type.  The options below will only be used if this box is checked.'),
    '#default_value' => variable_get('media_'. $type_name .'_override', null),
  );
  $form['media_'. $type_name .'_enabled'] = array(
    '#title' => t('Enable Media resource browser'),
    '#type' => 'checkbox',
    '#description' => t('Enable the Media resource browser for this node type.'),
    '#default_value' => variable_get('media_'. $type_name .'_enabled', null),
  );
  
  // Add the upload form as a type of faux field.
  if (is_array($type['extra']['attachments'])) {
    $type['fields']['attachments'] = array(
      'field_name' => 'attachments',
      'type' => 'attachments',
      'widget' => array(
        'label' => $type['extra']['attachments']['label'],
      ),
    );
  }
    
  // extract the fields for this node type
  foreach ((array)$type['fields'] as $field_name => $field) {
    // create the field identifier
    $form[$field['field_name']] = array(
      '#type' => 'fieldset',
      '#title' => t('Field name: !name', array('!name' => $field['widget']['label'])),
      '#collapsible' => 'true',
    );
    // build a form for each type of module that we have
    foreach (media_registration_kinds() as $kind) {
      // get all the kinds that match this field
      if ($registrations = media_get_fields($field['type'], $kind)) {
        $options = array();
        foreach ($registrations as $id => $registration) {
          $options[$field['field_name'] .'--'. $id] = $registration['name'] .': '. $registration['description'];
        }
        $form[$field['field_name']]['media_'. $type_name .'_'. $field['field_name'] .'_'. $kind] = array(
          '#title' => t('Enable !kind options for this field', array('!kind' => $kind)),
          '#description' => t('Choose which !kind options you would like to have enabled on this field', array('!kind' => $kind)),
          '#type' => 'checkboxes',
          '#options' => $options,
          '#default_value' => variable_get('media_'. $type_name .'_'. $field['field_name'] .'_'. $kind, array()),
        );
      }
    }
    // if we didn't get any additional data, remove this field form
    // this is ugly but hey, sue me
    if (count($form[$field['field_name']]) == 3) {
      unset($form[$field['field_name']]);
    }
  }

  return system_settings_form($form);
}