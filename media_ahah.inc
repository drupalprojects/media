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
function media_ahah_formatter_load($registration_id, $node_type, $field, $uri = NULL, $file_extension = NULL) {
  // Get the cache form data.
  $form = form_get_cache($_POST['form_build_id'], &$cached_form_state);

  $uri = isset($uri) ? $uri : $_POST['media_files'];
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
}


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

  $form['formatter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Format'),
  );
  return $form;
}

function media_formater_ahah_form_submit($form, &$form_state) {
  if (function_exists('dpm')) {
    dpm($form);
    dpm($form_state);
  }
}
