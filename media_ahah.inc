<?php
// $Id$

/**
 *  This function builds the form elements for the file formater based
 *  on the node type and field for settings and the file extension to get the
 *  formatters.
 *  @param string $registration_id
 *    The id of the registration creating this formatter.
 *  @param string $node_type
 *    The kind of node.
 *  @param string $field
 *    The machine name of the field being operated on.
 *  @param string $uri
 *    (optional) The uri to the item, or file://path if it is being uploaded.
 *  @param string $file_extension
 *    (optional) The kind of file that will be operated on.
 */
function media_ahah_formatter_load() { //$registration_id, $node_type, $field, $uri = NULL, $file_extension = NULL) {
  // The form is generated in an include file which we need to include manually.
  include_once 'modules/node/node.pages.inc';
  // We're starting in step #3, preparing for #4.
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  // Step #4.
  $form = form_get_cache($form_build_id, $form_state);

  // Preparing for #5.
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // Step #5.
  drupal_process_form($form_id, $form, $form_state);
  // Step #6 and #7 and #8.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  // Do whatever we need here for Step #9.
  // ...
  drupal_set_message('hooray!');

  // Step #10: Output any drupal messages and the new form.
  $media_tabs = $form[$field]['media']['media_tabs'];
  $output = theme('status_messages') . drupal_render($media_tabs);

  // Ensure any changes to Drupal.settings js are preserved.
  $javascript = drupal_add_js(NULL, NULL, 'header');

  // Final rendering callback.
  drupal_json(array(
    'status' => TRUE,
    'data' => $output,
    'settings' => call_user_func_array('array_merge_recursive', $javascript['setting']),
  ));
}
/*
  // Trying out the model postulate by QuickTabs.
  // See http://katbailey.net/blog/katherine/the-dual-aspect-of-drupal-forms-and-what-this-means-for-your-ahah-callback
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $_POST;
  $form['#redirect'] = FALSE;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  dpm($form_state);
  dpm($form);
  $media_tabs = $form[$field]['media']['media_tabs'];
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  dpm($form);
//   unset($media_tabs['#prefix'], $media_tabs['#suffix']); // Prevent duplicate wrappers.
  dpm($form);
  dpm($media_tabs);
  $javascript = drupal_add_js(NULL, NULL, 'header');
  drupal_json(array(
    'status'   => TRUE,
    'data'     => theme('status_messages') . drupal_render($media_tabs),
    'settings' => call_user_func_array('array_merge_recursive', $javascript['setting']),
  ));

  return;

  dpm($registration_id);
  dpm($node_type);
  dpm($field);
  dpm($uri);
  dpm($file_extension);
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $_POST;
  $form['#redirect'] = FALSE;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  $uri = $form_state['post']['media_files'];
  dpm('from media_ahah_formatter_load: '. $uri);
  $file_extension = isset($file_extension) ? $file_extension : pathinfo($uri, PATHINFO_EXTENSION);
dpm('file_extension: '. $file_extension);
  // Get the file creator for this item.
  $file_creator = media_get_registered_modules(array($registration_id));

  // Get the formaters for this node type.
  $formatters = media_active_fields_for_node_type($node_type, 'formatter');

  // Get the registrations.
  $registrations = media_get_registered_modules($formatters[$field]);

  // Remove any non-applying registrations.
  $registrations = media_get_applicable_formatters($registrations, $file_extension);

  // Get all the formatting forms.
  $formatter_options = array();
  $forms = array();
  foreach ($registrations as $id => $registration) {
    $formatter_options[$id] = $registration['name'];
    $function = $registration['callbacks']['form'];
    if (function_exists($function)) {
      $forms[$id] = $function($node_type, $field, $file_extension, $uri);
    }
  }
  dpm($forms);
//   $form_portion = $form['my_ahah_wrapper']['addable_elements'];
//   unset($form_portion['#prefix'], $form_portion['#suffix']); // Prevent duplicate wrappers.
  dpm($form);
  dpm($form_state);
  dpm($form_portion);
  $output = theme('status_messages') . drupal_render($form_portion);
  drupal_json(array('status' => TRUE, 'data' => $output));
return;

// Here's the original code we started from.

  // Get the cache form data.
  $form = form_get_cache($_POST['form_build_id'], &$cached_form_state);
  dpm($form);
  $uri = isset($uri) ? $uri : $_POST['media_files'];
  dpm('from media_ahah_formatter_load: '. $uri);
  $file_extension = isset($file_extension) ? $file_extension : pathinfo($uri, PATHINFO_EXTENSION);

  // Get the file creator for this item.
  $file_creator = media_get_registered_modules(array($registration_id));

  // Get the formaters for this node type.
  $formatters = media_active_fields_for_node_type($node_type, 'formatter');

  // Get the registrations.
  $registrations = media_get_registered_modules($formatters[$field]);

  // Remove any non-applying registrations.
  $registrations = media_get_applicable_formatters($registrations, $file_extension);

  // Get all the formatting forms.
  $formatter_options = array();
  $forms = array();
  foreach ($registrations as $id => $registration) {
    $formatter_options[$id] = $registration['name'];
    $function = $registration['callbacks']['form'];
    if (function_exists($function)) {
      $forms[$id] = $function($node_type, $field, $file_extension, $uri);
    }
  }
//   media_ahah_debug($formatter_options);
  return drupal_json(drupal_get_form('media_formater_ahah_form', $formatter_options, $forms, $uri));
}*/


function media_ahah_debug($array) {

  drupal_json('<pre>'. print_r($array, true) .'</pre>');
  exit();

}


/**
 *  AHAH form displayed under the file selector.
 *
 *  This pop-ups after the user selects or uploads a file, and then clicks the
 *  'Add' button. This AHAH form will do things like find metadata to associate
 *  with the file, and perhaps ask some information about how to format it.
 *
 *  It's important to note that we do not yet have a File Object (fid) at this
 *  stage, so we're working from an md5 hash, that Drupal will later compare to
 *  ensure we're still on the same page.
 *
 *  @param array $form_state
 *  @param array $options
 *  @param array $forms
 *  @param string $uri
 *    The file that's just been selected or uploaded.
 *  @return array
 *    drupal form array
 */
function media_formater_ahah_form($form_state, $options, $forms, $uri) {
  // Get the cache form data.
  $uri = isset($uri) ? $uri : $_POST['media_files'];
  $form['formatter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Format options'),
  );

  // If we only have one option, display that.
  if (count($options) > 1) {
    $form['formatter']['formatter_select'] = array(
      '#type' => 'select',
      '#title' => t('Select the formater to use'),
      '#options' => $options,
    );
  }
  $form['formatter'][] = $forms;

  // @TODO: Why won't this value stick?
  // We need to pass this for final submission.
  $form['uri'] = array(
    '#type' => 'value',
    '#value' => $uri,
  );

  dpm('from media_formater_ahah_form: '. $uri);

  $form['test'] = array(
    '#type' => 'value',
    '#value' => 'Stick, damn you!',
  );

  $form['formatter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Format'),
  );
  return $form;
}

function media_formater_ahah_form_submit($form, &$form_state) {
  drupal_set_message('media_formater_ahah_form_submit');
  if (function_exists('dpm')) {
    dpm($form);
    dpm($form_state);
  }
}


function media_build_browser_submit($form, &$form_state) {
  drupal_set_message('submitted: media_build_browser_submit');

  unset($form_state['submit_handlers']);
  form_execute_handlers('submit', $form, $form_state);
  $media_files = $form_state['values'];
  $form_state['media_files'] = $media_files;
  $form_state['rebuild'] = TRUE;

    $uri = $form_state['post']['media_files'];
  dpm('from media_ahah_formatter_load: '. $uri);
  $file_extension = isset($file_extension) ? $file_extension : pathinfo($uri, PATHINFO_EXTENSION);
dpm('file_extension: '. $file_extension);
  // Get the file creator for this item.
  $file_creator = media_get_registered_modules(array($registration_id));

  // Get the formaters for this node type.
  $formatters = media_active_fields_for_node_type($node_type, 'formatter');

  // Get the registrations.
  $registrations = media_get_registered_modules($formatters[$field]);

  // Remove any non-applying registrations.
  $registrations = media_get_applicable_formatters($registrations, $file_extension);

  // Get all the formatting forms.
  $formatter_options = array();
  $forms = array();
  foreach ($registrations as $id => $registration) {
    $formatter_options[$id] = $registration['name'];
    $function = $registration['callbacks']['form'];
    if (function_exists($function)) {
      $forms[$id] = $function($node_type, $field, $file_extension, $uri);
    }
  }
  drupal_set_message(t('media_build_browser_submit: now we need to attach the file !uri and collect metadata.', array('!uri' => l($uri, $uri))));
  return $forms;
}

