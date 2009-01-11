<?php

// $Id$

/**
 * @file
 * This file contains the admin functions for the media module
 */


/**
 * provides the per content type settings
 *
 * @return unknown
 */
function media_admin_settings($form, $type_name = null) {
  $form = array();
  
  // default page
  if (! $type_name) {
    $form['admin'] = array(
      '#value' => t('Media module settings')
    );    
    // get all the content types
    $types = node_get_types();
    if ($types) {
      $content_types = array();      
      foreach ($types as $content_type) {
        $content_types[] = l($content_type->name, 'admin/content/media/'. $content_type->type);
      }      
      $form['types'] = array(
        '#value' => theme('item_list', $content_types),
      );
    }
    return system_settings_form($form);    
  }  
  
  // per content type settings
  
  // do we have cck installed?
  if (module_exists('content')) {
    $type = content_types($type_name);  
  }
  else {
    // get the specific content type
    $type = node_get_types('type', $type_name);
  }
  
  // master  settings
  $form['media_'. $type_name .'_enabled'] = array(
    '#title' => t('Media resource browser'),
    '#type' => 'checkbox',
    '#descripton' => t('Enable the Media resource browser for this node type.'),
    '#default_value' => variable_get('media_'. $type_name .'_enabled', null),
  );
  
  // extract the fields for this node type
  foreach ($type['fields'] as $field) {        
    $form[$field['field_name']] = array(
      '#value' => t('Field name: !name', array('!name' => $field['widget']['label'])),
    );

    // build a form for each type of module that we have  
    foreach (media_registration_kinds() as $kind) {
      // get all the kinds that match this field
      if ($registrations = media_get_fields($field['type'], $kind)) {
        $options = array();
        foreach ($registrations as $id => $registration) {
          $options[$field['field_name'] .'--'. $id] = $registration['name'] .': '. $registration['description'];
        }
        $form[$field['field_name']]['media_'. $type_name .'_'. $kind] = array(
          '#title' => t('Enable !kind options for this field', array('!kind' => $kind)),
          '#description' => t('Choose which !kind options you would like to have enabled on this field', array('!kind' => $kind)),
          '#type' => 'checkboxes',
          '#options' => $options,
          '#default_value' => variable_get('media_'. $type_name .'_'. $kind, array()),
        );
      }       
    }
  }
    
  return system_settings_form($form);
}