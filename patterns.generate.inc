<?php 

function patterns_generate_variables(&$form_state) {

  require_once(drupal_get_path('module', 'system') .'/system.admin.inc');
  $modules = system_modules();
  foreach ($modules['validation_modules']['#value'] as $name => $module_obj) {
    $module_options[$name] = $module_obj->info['name'];
  }
  
  if ($_SESSION['patterns_generate_variables_state']) {
    $default_options['modules'] = unserialize($_SESSION['patterns_generate_variables_state']);
    unset($_SESSION['patterns_generate_variables_state']);
  }
  else {
    $default_options['modules'] = array();  // for variables, not checked by default
  }
  
  if  ($_SESSION['patterns_generate_pattern']) {
    $form['patterns_generate'] = array(
      '#type' => 'fieldset',
      '#title' => t('Pattern Result'),
      '#collapsible' => TRUE,
      );       
    $form['patterns_generate']['pattern'] = array(
      '#type' => 'textarea',
      '#title' => t('Pattern'),
      '#rows' => 20,
      '#default_value' => $_SESSION['patterns_generate_pattern'],
      '#description' => "Cut and paste this where you want to.
         Maybe eyeball it to see if its what you want."
      );
    unset($_SESSION['patterns_generate_pattern']);
  }
    
  $form['modules'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Module\'s Variables to Include in Pattern'),
    '#required' => True,
    '#default_value' => $default_options['modules'],
    '#options' => $module_options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Variables Pattern for selected Modules')
  );

  $form['#validate'][] = 'patterns_generate_variables_validate';
  $form['#submit'][] = 'patterns_generate_variables_submit';

  return $form;
}

function patterns_generate_variables_validate($form, &$form_state) {
  //  validation as needed
}

function patterns_generate_variables_submit($form, &$form_state) {

  $modules = $form_state['values']['modules'];
  
  $actions = patterns_generate_variables_template($modules);
  $pattern = patterns_generate_template($actions, $modules);

  $_SESSION['patterns_generate_pattern'] = $pattern;
  $_SESSION['patterns_generate_variables_state'] = serialize($form_state['values']['modules']);
  $form_state['redirect'] = 'admin/build/patterns/generate/variables';
  return;
}


/**
 * generate a pattern for as much information as possible about a single module
 *  (guessing at what is part of the module by variable name beginnings)
 */
function patterns_generate_module(&$form_state) {

  $user_roles = user_roles();
    
  require_once(drupal_get_path('module', 'system') .'/system.admin.inc');
  $modules = system_modules();
  foreach ($modules['validation_modules']['#value'] as $name => $module_obj) {
    $module_options[$name] = $module_obj->info['name'];
  }
  
  if ($_SESSION['patterns_generate_module_state']) {
    $default_options = unserialize($_SESSION['patterns_generate_module_state']);
    unset($_SESSION['patterns_generate_module_state']);
  }
  else {
    $default_options['rolesandperms']['roles'] = array_combine(  array_keys($user_roles) , array_keys($user_roles)  ); 
    $default_options['rolesandperms']['overwrite'] = array();
    $default_options['include']['variables'] = array(1 => 'Variables');
    $default_options['modules']['module'] = "";
  }
  
  if  ($_SESSION['patterns_generate_pattern']) {
    $form['patterns_generate'] = array(
      '#type' => 'fieldset',
      '#title' => t('Pattern Result'),
      '#collapsible' => TRUE,
      );       
    $form['patterns_generate']['pattern'] = array(
      '#type' => 'textarea',
      '#title' => t('Pattern'),
      '#rows' => 20,
      '#default_value' => $_SESSION['patterns_generate_pattern'],
      '#description' => "Cut and paste this where you want to.
         Maybe eyeball it to see if its what you want."
      );
    unset($_SESSION['patterns_generate_pattern']);
  }

  $form['modules'] = array(
      '#type' => 'fieldset',
      '#title' => t('Module'),
      '#collapsible' => TRUE,
  );

  $form['modules']['module'] = array(
    '#type' => 'select',
    '#title' => t('Module to generate Pattern for'),
    '#required' => True,
    '#default_value' => $default_options['module'],
    '#options' => $module_options,
    '#description' => 'Select a module to generate a pattern from.  A
      starting point for a Pattern for that module will be generated.  Currently this
      is related variables and roles.  Roles will not be that useful unless you use them consistantly
      throughout your Drupal instances.'
  );
      
  $form['rolesandperms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Roles to Generate Permissions for'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#description' => "Check the roles and module permissions you want emulate in the generated pattern.
      For every role and module selected, a pattern to reflect <a href=\"/admin/user/permissions\">current permissions</a>
      and roles on this site will be generated. Leave blank to not generate permissions. "
  );

  $form['rolesandperms']['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles to Include in Pattern'),
    '#required' => True,
    '#options' => $user_roles,
    '#default_value' => $default_options['rolesandperms']['roles'],
  );

 $form['rolesandperms']['overwrite'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Overwrite Permissions'),
    '#required' => FALSE,
    '#default_value' => $default_options['rolesandperms']['overwrite'],
    '#description' => "overwrite will cause all other permissions for each role 
      to be reset/disabled",
    '#options' => array('1' => 'Overwrite'),
  );

  $form['include'] = array(
    '#type' => 'fieldset',
    '#title' => t('Actions to generate'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#description' => "Which actions to generate?"
  );

  $form['include']['variables'] = array(
    '#type' => 'checkboxes',
    '#required' => FALSE,
    '#default_value' => $default_options['include']['variables'],
    '#description' => "This option will select all variables possibly related to the
    selected module based on the query <code>select name,value from variable where name like 'user%'</code>
    (in the case of the user module and generate patterns for them.",
    '#options' => array('1' => 'Variables'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Starting Pattern for Selected Module')
  );

  $form['#validate'][] = 'patterns_generate_module_validate';
  $form['#submit'][] = 'patterns_generate_module_submit';

  return $form;
}

function patterns_generate_module_validate($form, &$form_state) {

}


function patterns_generate_module_submit($form, &$form_state) {

  $overwrite = $form_state['values']['rolesandperms']['overwrite'][1]; // overwrite = 1 || 0
  $rids = $form_state['values']['rolesandperms']['roles'];
  $module_id = $form_state['values']['module'];
  $modules = array( $module_id => $module_id);
  $variables = $form_state['values']['variables'];
  
  $actions = patterns_generate_variables_template(array($module_id => TRUE));
  $actions = array_merge($actions, patterns_generate_rolesandperms_template($modules, $overwrite, $rids));
  $pattern = patterns_generate_template($actions, $modules);

  $_SESSION['patterns_generate_pattern'] = $pattern;
  $_SESSION['patterns_generate_module_state'] = serialize($form_state['values']); 
  $form_state['redirect'] = 'admin/build/patterns/generate/module';
  return;
}



function patterns_generate_variables_template($modules) {
  
  $mod_list = array_filter($modules);
  $sql = 'SELECT name,value FROM {variable} WHERE '. str_repeat(' name like "%s%" OR ', count($mod_list));
  $sql = trim($sql, "OR ");

  $result = db_query($sql, array_keys($mod_list)); 
  $parser = xml_parser_create('');
  while ($variable = db_fetch_array($result)) {
    $value = trim(unserialize($variable['value']));
    if ( (! $value ) || is_null($value) || is_numeric($value) || is_bool($value) ) {
      // do nothing, but don't parse xml either
    }
    elseif (! @simplexml_load_string("<?xml version='1.0'?>\n<document>$value</document>\n")) { 
      $value = "\n<![CDATA[$value \n]]>";
    }
    $actions[] = '    <variable name="'. $variable['name']  .'">'. $value ."</variable>";
  }
  return $actions;
}


  
/**
 * generate a pattern for roles and perms based on subset of current site's roles and perms
 */
function patterns_generate_rolesandperms(&$form_state) {

  $user_roles = user_roles();
    
  require_once(drupal_get_path('module', 'system') .'/system.admin.inc');
  $modules = system_modules();
  foreach ($modules['validation_modules']['#value'] as $name => $module_obj) {
    $module_options[$name] = $module_obj->info['name'];
  }

  if ($_SESSION['patterns_generate_rolesandperms_state']) {
    $default_options = unserialize($_SESSION['patterns_generate_rolesandperms_state']);
    unset($_SESSION['patterns_generate_rolesandperms_state']);
  }
  else {
    $default_options['rolesandperms']['roles'] = array_combine(  array_keys($user_roles) , array_keys($user_roles)  ); 
    $default_options['rolesandperms']['modules']= array_combine(  array_keys($module_options) , array_keys($module_options)  ); 
    $default_options['rolesandperms']['overwrite'] = array();
  }
  
  if  ($_SESSION['patterns_generate_pattern']) {
    $form['patterns_generate'] = array(
      '#type' => 'fieldset',
      '#title' => t('Pattern Result'),
      '#collapsible' => TRUE,
      );       
    $form['patterns_generate']['pattern'] = array(
      '#type' => 'textarea',
      '#title' => t('Pattern'),
      '#rows' => 20,
      '#default_value' => $_SESSION['patterns_generate_pattern'],
      '#description' => "Cut and paste this where you want to.
         Maybe eyeball it to see if its what you want."
      );
    unset($_SESSION['patterns_generate_pattern']);
  }
    
  $form['rolesandperms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Generate Role and Permission Pattern'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#description' => "Check the roles and module permissions you want emulate in the generated pattern.
    For every role and module selected, a pattern to reflect <a href=\"/admin/user/permissions\">current permissions</a>
    and roles on this site will be generated."
  );

  $form['rolesandperms']['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles to Include in Pattern'),
    '#required' => True,
    '#options' => user_roles(),
   '#default_value' => $default_options['rolesandperms']['roles'],
  );
    
  $form['rolesandperms']['overwrite'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Overwrite Permissions'),
   '#required' => FALSE,
    '#default_value' => $default_options['rolesandperms']['overwrite'],
    '#description' => "overwrite will cause all other permissions for each role 
      to be reset/disabled",
    '#options' => array('1' => 'Overwrite'),
  );

  $form['rolesandperms']['modules'] = array(
   '#type' => 'checkboxes',
    '#title' => t('Module Permissions to Include in Pattern'),
    '#required' => True,
    '#default_value' => $default_options['rolesandperms']['modules'],
    '#options' => $module_options,
  );

  $form['submit'] = array(
   '#type' => 'submit',
    '#value' => t('Generate Roles and Permission Pattern')
  );

  $form['#validate'][] = 'patterns_generate_rolesandperms_validate';
  $form['#submit'][] = 'patterns_generate_rolesandperms_submit';

 return $form;
}

function patterns_generate_rolesandperms_validate($form, &$form_state) {

}

function patterns_generate_rolesandperms_submit($form, &$form_state) {

  $overwrite = $form_state['values']['rolesandperms']['overwrite'][1]; // overwrite = 1 || 0
  $rids = $form_state['values']['rolesandperms']['roles'];
  $modules = $form_state['values']['rolesandperms']['modules'];
  
  $actions = patterns_generate_rolesandperms_template($modules, $overwrite, $rids); 
 $pattern = patterns_generate_template($actions, $modules);

  $_SESSION['patterns_generate_pattern'] = $pattern;
  $_SESSION['patterns_generate_rolesandperms_state'] = serialize($form_state['values']);
  $form_state['redirect'] = 'admin/build/patterns/generate/rolesandperms';
 return;
}


function patterns_generate_rolesandperms_template($modules, $overwrite, $rids) {
 foreach ($rids as $rid => $use_rid) {
    if ($use_rid) {
      $rids_requested[] = $rid;
    } 
  }
  
 $result = db_query('SELECT r.rid, p.perm FROM {role} r LEFT JOIN {permission} p ON r.rid = p.rid WHERE r.rid in (%s)',
    join(',', $rids_requested) );
  
  while ($role = db_fetch_object($result)) {
    $role_permissions[$role->rid] = $role->perm .',';
  }
/* snipped below modified from user_admin_perm  from modules/user/user.admin.inc */
  foreach ($modules as $module => $use_module) {
    if ($use_module && $permissions = module_invoke($module, 'perm')) {
      asort($permissions);
      foreach (array_values($permissions) as $perm) {
        foreach ($rids_requested as $rid) {
          if (strpos($role_permissions[$rid], $perm .',') !== FALSE) {
            $perms_filtered_by_requested_rid[$rid][] = $perm;
          }
        }
      }
   }
  }
 
  $roles = user_roles();
  

// could be themeable but not sure of utility of that
  foreach ($perms_filtered_by_requested_rid as $rid => $perms) {
    $role_name = $roles[$rid];
    if (count($perms) > 0) {
        $perm_items = "      <value>". join("</value>\n      <value>", $perms) ."</value>\n";
        $perms_xml .= "    <permissions role=\"$role_name\" overwrite=\"$overwrite\" >\n$perm_items    </permissions>\n\n";
    }
  }
  $roles_names = array_flip($roles);
  unset($roles_names['anonymous user']);
  unset($roles_names['authenticated user']);
  $roles_xml = "    <role>". join("</role>\n    <role>", array_keys($roles_names) ) ."</role>\n";
  
  $actions = array($roles_xml, $perms_xml);
  return $actions;

}

function patterns_generate_template($actions, $required_modules) {

  foreach ($required_modules as $name => $use) {
    if ($use) {
      $modules[] = $name;
      }
  }
  
  if (count($modules) > 0) {
    $modules_xml = "\n  <modules>\n    <module>". join("</module>\n    <module>", $modules) ."</module>\n  </modules>\n";
  }
  $actions_xml = join("\n", $actions);
    return   <<<EOF
?xml version="1.0" encoding="ISO-8859-1"?>
<pattern>
  <info>
    <title></title>
    <description></description>
    <author></author>
    <author_email></author_email>
    <version></version>
    <category></category>
    <core>6.x</core>    
 </info>
$modules_xml 
  <actions>

$actions_xml
  </actions>
</pattern>
EOF;

}

