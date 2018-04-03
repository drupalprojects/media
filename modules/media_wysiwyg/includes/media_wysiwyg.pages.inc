<?php

/**
 * @file
 * Common pages for the Media WYSIWYG module.
 */

/**
 * Form callback for embedding media.
 *
 * Allows the user to pick a view mode (format) and various other settings for a
 * media instance. The values from this form follows the media token schema and
 * its values are mapped directly to media tokens.
 */
function media_wysiwyg_format_form($form, &$form_state, $file) {
  $form_state['file'] = $file;

  // Build instance settings from token schema defaults and anything from query
  // parameters (ajax).
  $instance_settings = array_map(function ($property) {
    return $property['default'];
  }, media_wysiwyg_schema_token());

  if (isset($_GET['instance_settings'])) {
    $instance_settings = array_merge($instance_settings, drupal_json_decode($_GET['instance_settings']));
  }
  // For new media instances this array is the entire file entity form,
  // including file type ('type'), which conflict with the 'type' property of
  // our media token. Adjust our settings to adhere to the media token schema.
  $instance_settings['type'] = 'media';
  $instance_settings['fid'] = $file->fid;
  $instance_settings['file_type'] = $file->type;

  // Build view mode options, assert that we have a valid view mode in instance
  // settings and build a rendered preview of each one of them.
  $view_mode_options = media_wysiwyg_get_file_view_mode_options($file);
  if (!count($view_mode_options)) {
    throw new Exception('Unable to continue, no available formats for displaying media.');
  }
  if (!isset($view_mode_options[$instance_settings['view_mode']])) {
    reset($view_mode_options);
    $instance_settings['view_mode'] = key($view_mode_options);
  }
  if (!isset($form_state['storage']['view_mode_previews'])) {
    $form_state['storage']['view_mode_previews'] = array();
    foreach ($view_mode_options as $view_mode => $view_mode_label) {
      $view_mode_preview = media_wysiwyg_get_file_without_label($file, $view_mode, array('wysiwyg' => TRUE));
      $form_state['storage']['view_mode_previews'][$view_mode] = drupal_render($view_mode_preview);
    }
  }
  // Add the previews by reference so that they can easily be altered by
  // changing $form['#formats'].
  $form['#formats'] = &$form_state['storage']['view_mode_previews'];

  // Add the required libraries, JavaScript, settings and CSS for the form.
  $us = drupal_get_path('module', 'media_wysiwyg');
  $js_settings = array('media' => array('formatFormFormats' => &$form_state['storage']['view_mode_previews']));
  $form['#attached']['library'][] = array('media', 'media_base');
  $form['#attached']['library'][] = array('system', 'form');
  $form['#attached']['css'][] = $us . '/css/media_wysiwyg.css';
  $form['#attached']['js'][] = $us . '/js/media_wysiwyg.format_form.js';
  $form['#attached']['js'][] = array('data' => $js_settings, 'type' => 'setting');

  $form['title'] = array(
    '#markup' => t('Embedding %filename', array('%filename' => $file->filename)),
  );
  $preview = media_get_thumbnail_preview($file);
  $form['preview'] = array(
    '#type' => 'markup',
    '#markup' => drupal_render($preview),
  );

  // Everything under $form['options'] will get passed to the WYSIWYG
  // environment and used as media instance settings which in turn are
  // serialized to actual media tokens.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('options'),
  );
  // Add the non-editable variables of media tokens.
  foreach (array('type', 'fid', 'file_type') as $property) {
    $form['options'][$property] = array(
      '#type' => 'hidden',
      '#value' => $instance_settings[$property],
    );
  }

  // @TODO: Display more verbose information about which formatter and what it
  // does.
  $form['options']['view_mode'] = array(
    '#type' => 'select',
    '#title' => t('Display as'),
    '#options' => $view_mode_options,
    '#default_value' => $instance_settings['view_mode'],
    '#description' => t('Choose the type of display you would like for this
      file. Please be aware that files may display differently than they do when
      they are inserted into an editor.'),
  );
  if (variable_get('media_wysiwyg_alignment', FALSE)) {
    $form['options']['alignment'] = array(
      '#type' => 'select',
      '#title' => t('Alignment'),
      '#options' => array(
        '' => t('None'),
        'left' => t('Left'),
        'right' => t('Right'),
        'center' => t('Center'),
      ),
      '#description' => t('Choose how you would like the media to be aligned with surrounding content.'),
      '#default_value' => $instance_settings['alignment'],
    );
  }

  // Add fields from the file, so that we can override them if necessary.
  $fields = media_wysiwyg_filter_field_parser($instance_settings, $file);
  foreach ($fields as $field_name => $field_value) {
    $file->{$field_name} = $field_value;
  }
  $form['options']['fields'] = array();
  field_attach_form('file', $file, $form['options']['fields'], $form_state);
  foreach (field_info_instances('file', $file->type) as $field_name => $field_instance) {
    $info = field_info_field($field_name);
    $allow = !empty($field_instance['settings']['wysiwyg_override']);
    // Only single valued fields can be overridden normally, unless Media is
    // configured otherwise with "media_wysiwyg_wysiwyg_override_multivalue".
    if ($allow && $info['cardinality'] != 1) {
      $allow = variable_get('media_wysiwyg_wysiwyg_override_multivalue', FALSE);
    }
    $form['options']['fields'][$field_name]['#access'] = $allow;
  }

  // Field to attach external url's to files for linking.
  if (variable_get('media_wysiwyg_external_link', FALSE)) {
    if ($file->type == 'image') {
      $form['options']['external_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Link Image'),
        '#description' => t('Enter a URL to turn the image into a link.'),
        '#default_value' => $instance_settings['external_url'],
      );
    }
  }

  // Add view mode preview.
  media_wysiwyg_format_form_view_mode($form, $form_state, $file, $instance_settings);

  // Similar to a form_alter, but we want this to run first so that
  // media.types.inc can add the fields specific to a given type (like alt tags
  // on media). If implemented as an alter, this might not happen, making other
  // alters not be able to work on those fields.
  // @todo: We need to pass in existing values for those attributes.
  drupal_alter('media_wysiwyg_format_form_prepare', $form, $form_state, $file);

  return $form;
}

/**
 * Add ajax preview when selecting view mode in wysiwyg editor.
 */
function media_wysiwyg_format_form_view_mode(&$form, $form_state, $file, $instance_settings) {
  // Find view mode for this media item, either from form-submitted values or
  // the already verified mode from instance_settings.
  $view_mode = empty($form_state['values']['view_mode']) ? $instance_settings['view_mode'] : $form_state['values']['view_mode'];

  $link_options = array(
    'attributes' => array(
      'class' => 'button',
      'title' => t('Use for replace fox or edit file fields.'),
    ),
  );
  if (!empty($_GET['render'])) {
    $link_options['query']['render'] = $_GET['render'];
  }

  $form['preview'] = array();
  $form['preview']['#prefix'] = '<div class="media-preview-group"><div class="media-item"><div class="media-thumbnail">';
  $form['preview']['#suffix'] = '</div><div class="label-wrapper"><label class="media-filename">' . check_plain($file->filename) . '</label></div></div><div class="edit-file-link">' . l(t('Edit file'), 'file/' . $file->fid . '/edit', $link_options) . '</div></div>';
  $form['preview']['thumbnail'] = file_view_file($file, $view_mode);
  $form['preview']['thumbnail']['#prefix'] = '<div id="media-preview">';
  $form['preview']['thumbnail']['#suffix'] = '</div>';

  if (!isset($form['options']['view_mode']['#default_value'])) {
    $form['options']['view_mode']['#default_value'] = $view_mode;
  }
  $form['options']['view_mode']['#ajax'] = array(
    'callback' => 'media_wysiwyg_format_form_preview',
    'wrapper' => 'media-preview',
  );

  $wysiwyg_view_mode = db_query('SELECT view_mode FROM {media_view_mode_wysiwyg} WHERE type = :type', array(':type' => $file->type))->fetchField();
  $view_modes = media_wysiwyg_get_wysiwyg_allowed_view_modes($file);
  $formats = $options = array();

  foreach ($view_modes as $view_mode => $view_mode_info) {
    $options[$view_mode] = $view_mode_info['label'];

    if (!empty($wysiwyg_view_mode)) {
      $element = media_wysiwyg_get_file_without_label($file, $wysiwyg_view_mode, array('wysiwyg' => TRUE));
    }
    else {
      $element = media_wysiwyg_get_file_without_label($file, $view_mode, array('wysiwyg' => TRUE));
    }

    $formats[$view_mode] = drupal_render($element);
  }

  $form['#formats'] = $formats;
  $form['options']['view_mode']['#options'] = $options;
}

/**
 * AJAX callback to select portion of format form to be updated with a preview.
 *
 * @param array $form
 *   An associative array containing the structure of the form.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 *
 * @return array
 *   The preview form item.
 */
function media_wysiwyg_format_form_preview($form, $form_state) {
  return $form['preview']['thumbnail'];
}

/**
 * Upgrade a single media token to latest version.
 *
 * @param array $tag_info
 *   The media token as a json-decoded array.
 */
function media_wysiwyg_upgrade_token(array &$tag_info) {
  $version = variable_get('media_wysiwyg_token_version', '');
  if (version_compare($version, '3.0', '<')) {
    media_wysiwyg_aggregate_alignment($tag_info);
  }
  // Successive upgrades follows here.
}

/**
 * Upgrade media tokens in filtered text fields for a given entity.
 *
 * Only upgraded text fields will be updated in storage, omitting the costly
 * entity_save(). I.e. no new entity revisions.
 *
 * @param string $entity_type
 *   The entity type.
 * @param int $id
 *   The entity ID to scan for media tokens in.
 *
 * @return array
 *   The number of found and upgraded tokens in entity, keyed by 'found' and
 *   'upgraded'.
 */
function media_wysiwyg_upgrade_entity_tokens($entity_type, $id) {
  $report = array(
    'found' => 0,
    'upgraded' => 0,
  );

  if (!($entity = entity_load($entity_type, array($id)))) {
    return $report;
  };
  $entity = reset($entity);

  list(,, $bundle) = entity_extract_ids($entity_type, $entity);

  // Map between storage engines and the fields for it that will be updated.
  $storages = array();
  foreach (media_wysiwyg_filter_fields_with_text_filtering($entity_type, $entity) as $field_name) {
    $langcode = field_language($entity_type, $entity, $field_name);
    if (!isset($entity->{$field_name}[$langcode])) {
      continue;
    }
    $field = field_info_instance($entity_type, $field_name, $bundle);
    $field = field_info_field_by_id($field['field_id']);
    $field_id = $field['id'];
    foreach ($entity->{$field_name}[$langcode] as &$field_item) {
      if (empty($field_item['value'])) {
        continue;
      }
      preg_match_all(MEDIA_WYSIWYG_TOKEN_REGEX, $field_item['value'], $matches);
      foreach ($matches[0] as $tag_orig) {
        $tag_orig = str_replace(array('[[', ']]'), '', $tag_orig);
        $tag_info = drupal_json_decode($tag_orig);
        if (!isset($tag_info['type']) || $tag_info['type'] != 'media') {
          continue;
        }
        $report['found']++;
        // Perform the actual upgrade.
        media_wysiwyg_upgrade_token($tag_info);
        $tag_new = drupal_json_encode($tag_info);
        if (strcmp($tag_orig, $tag_new) == 0) {
          // No changes. Don't bother saving this.
          continue;
        }
        $report['upgraded']++;
        $tag_orig = '[[' . $tag_orig . ']]';
        $tag_new = '[[' . $tag_new . ']]';
        // The field_item is available by reference, so it will updated in the
        // entity directly. If several identical tokens exists within this text
        // value they will all be replaced here, and the next match iteration
        // will not perform any replacement as it the search string will not be
        // found. No big deal and no need to add a special case for this.
        $field_item['value'] = str_replace($tag_orig, $tag_new, $field_item['value']);
        $storages[$field['storage']['type']][$field_id] = $field_id;
      }
    }
  }
  // Write updated tokens to storage.
  foreach ($storages as $storage => $fields) {
    $storage_info = field_info_storage_types($storage);
    module_invoke($storage_info['module'], 'field_storage_write', $entity_type, $entity, FIELD_STORAGE_UPDATE, $fields);
  }
  return $report;
}

/**
 * Upgrade media tokens in content.
 *
 * Over time the format of media tokens will change and this function will
 * assert that existing tokens in content will remain up to date with todays
 * standard.
 *
 * It examines the {file_usage} table and searches all entities for filtered
 * text fields with macros. All found media tokens will be upgraded to the
 * latest token format version and the individual text fields will be updated in
 * db directly, omitting all hooks otherwise run during entity update.
 *
 */
function media_wysiwyg_upgrade_content_tokens() {
  $batch = array(
    'title' => t("Upgrading media tokens"),
    'operations' => array(
      array(
        'media_wysiwyg_upgrade_content_tokens_batch',
        array(),
      ),
    ),
    'finished' => 'media_wysiwyg_upgrade_content_tokens_finish',
    'file' => drupal_get_path('module', 'media_wysiwyg') . '/includes/media_wysiwyg.pages.inc',
  );
  batch_set($batch);
  batch_process('admin/reports/status');
}

/**
 * Batch API callback.
 *
 * @see media_wysiwyg_upgrade_content_tokens()
 */
function media_wysiwyg_upgrade_content_tokens_batch(&$context) {
  $items_per_run = 20;

  $query = db_select('file_managed', 'fm');
  $query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
  $query->fields('fu', array('type', 'id'))
    ->distinct();
  if (empty($context['results']['!entities_total'])) {
    // The countQuery() returns a cloned SelectQuery object, so we can happily
    // execute it without disturbing the original query. Also, the keys in the
    // 'results' entry are prefixed with '!' so the $results variable can be
    // sendt directly to t() in the batch finished callback.
    $context['results']['!entities_total'] = $query->countQuery()->execute()->fetchField();
    $context['results']['!entities_upgraded'] = 0;
    $context['results']['!tokens_total'] = 0;
    $context['results']['!tokens_upgraded'] = 0;

    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_type'] = '';
    $context['sandbox']['current_id'] = 0;
    $context['sandbox']['run'] = 0;
  }
  $query->orderBy('fu.type')
    ->orderBy('fu.id')
    ->range($context['sandbox']['run'] * $items_per_run, $items_per_run);
  foreach ($query->execute() as $usage) {
    $context['sandbox']['progress']++;
    $context['sandbox']['current_id'] = $usage->id;
    $context['sandbox']['current_type'] = $usage->type;
    $context['message'] = t("Upgrading tokens: Processing entity # !progress/!total. Last examined: !entity_type/!entity_id", array(
      '!progress' => $context['sandbox']['progress'],
      '!total' => $context['results']['!entities_total'],
      '!entity_type' => $usage->type,
      '!entity_id' => $usage->id,
    ));
    try {
      $report = media_wysiwyg_upgrade_entity_tokens($usage->type, $usage->id);
    }
    catch (Exception $e) {
      watchdog('media', "Failed to upgrade tokens in entity %entity_id with id %entity_id", array('%entity_type' => $usage->type, '%entity_id' => $usage->id), WATCHDOG_WARNING);
    }
    $context['results']['!tokens_total'] += $report['found'];
    $context['results']['!tokens_upgraded'] += $report['upgraded'];
    if ($report['upgraded']) {
      $context['results']['!entities_upgraded']++;
    }
  }
  if ($context['sandbox']['progress'] < $context['results']['!entities_total']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['results']['!entities_total'];
  }
  $context['sandbox']['run']++;
}

/**
 * Batch API finish callback.
 *
 * @see media_wysiwyg_upgrade_content_tokens()
 */
function media_wysiwyg_upgrade_content_tokens_finish($success, $results, $operations) {
  if ($success) {
    $results['!version'] = MEDIA_WYSIWYG_TOKEN_VERSION;
    drupal_set_message(t("Media token version !version upgrade summary: <ul><li>Entities inspected: !entities_total</li> <li>Media tokens found: !tokens_total.</li> <li>Tokens upgraded: !tokens_upgraded</li> <li>Entities affected: !entities_upgraded</li></ul>", $results));
    variable_set('media_wysiwyg_token_version', MEDIA_WYSIWYG_TOKEN_VERSION);
    // Instead of using the costly drupal_flush_all_caches() we flush relevant
    // core caches and invoke hook_flush_caches() to allow content cache modules
    // (entitycache) to flush whatever needed.
    $core = array('cache', 'cache_filter', 'cache_page', 'cache_block');
    $cache_tables = array_merge(module_invoke_all('flush_caches'), $core);
    foreach ($cache_tables as $table) {
      cache_clear_all('*', $table, TRUE);
    }
  }
  else {
    drupal_set_message(t('Media token upgrade failed. See system log'), 'warning');
  }
}
