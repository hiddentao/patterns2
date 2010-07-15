<?php
/**
 * Component for updating filters.
 */

function filter_patterns($op, $id = null, &$data = null) {
  switch($op) {
    // Return the valid tags that this component can prepare and process
    case 'tags':
      return array('format');
    break;

    // Return a list of forms/actions this component can handle
    case 'actions':
      return array(
        'filter_admin_format_form' => t('Filter: Add/configure input format'),
      );
    break;

    // Return a summary of an action
    case 'summary':
      if ('filter_admin_format_form' == $id) {
        return t('Configuring input filter: %name', array('%name' => $data['name']));
      }
    break;

    // Prepare data for processing
    case 'prepare':

    	$formats = filter_formats();
    	if (isset($data['id']) && isset($formats[$data['id']])) {
        $data['format_obj'] = $formats[$data['id']];
        if (!isset($data['name']))
           $data['name'] = $data['format_obj']->name;
    		$data['format'] = $data['id'];
    	} else if (isset($data['name'])) {
    		foreach ($formats as $id => $format) {
    			if ($data['name'] == $format->name) {
    				$data['format_obj'] = $format;
    				$data['format'] = $id;
    				break;
    			}
    		}
    	}
    break;

    // Pre validate actions
    case 'pre-validate':
      if (!isset($data['format']) && !isset($data['name'])) {
        return t('Valid filter "name" or "id" is required.');
      }
    break;

    // Return the form_id('s) for each action
    case 'form_id':
    	return 'filter_admin_format_form';
    	
    break;

    // Prepare for valid processing of this type of component
    case 'build':
    	if ('filter_admin_format_form' == $id) {
        module_load_include('inc', 'filter', 'filter.admin');
        
        if (!empty($data['roles']) && is_array($data['roles'])) {
	        $all_roles = array_flip(user_roles());
	        $role_ids = array();
	        foreach ($data['roles'] as $role_name) {
	        	$role_ids[$all_roles[$role_name]] = 1;
	        }
	        $data['roles'] = $role_ids;
        }

        if (!empty($data['filters']) && is_array($data['filters'])) {
	        $filters_all = filter_list_all();
	        $filters_ids = array();
	        foreach ($filters_all as $id => $f) {
	          $filters_ids[$f->name] = $id;
	        }
	        $enabled_filters = array();
	        foreach ($data['filters'] as $filter) {
	        	$enabled_filters[$filters_ids[$filter]] = 1;
	        }
          $data['filters'] = $enabled_filters;
        }        
        
      }
      return $data;
    break;

    // Validate the values for an action before running the pattern
    //
    // Check here rather than in pre-validate in case we're adding 
    // roles or filters in the same pattern.
    //
    case 'validate':
    	// check roles
      if (!empty($data['roles']) && is_array($data['roles'])) {
        $role_ids = array_flip(user_roles());
        $bad_roles = array();
        foreach ($data['roles'] as $role) {
          if (!isset($role_ids[$role])) {
            $bad_roles[] = $role;
          }
        }
        if (!empty($bad_roles)) {
        	return t('Invalid role names: %roles', array('%roles' => implode(', ',$bad_roles)));
        }
      }
      
      // check filters
      if (!empty($data['filters']) && is_array($data['roles'])) {
      	$filters_all = filter_list_all();
      	$filters_ids = array();
      	foreach ($filters_all as $id => $f) {
      		$filters_ids[$f->name] = $id;
      	}
        $bad_filters = array();
        foreach ($data['filters'] as $filter) {
          if (!isset($filters_ids[$filter])) {
            $bad_filters[] = $filter;
          }
        }
        if (!empty($bad_filters)) {
          return t('Invalid filter names: %filters', array('%filters' => implode(', ',$bad_filters)));
        }
      }
      
    break;

    // Build a patterns actions and parameters
    case 'params':
    	$params = array();
      if ('filter_admin_format_form' == $id) {
      	$params['format'] = isset($data['format_obj']) ? $data['format_obj'] : NULL; 
      	unset($data['format_obj']);
      }
      return $params;
    break;

    // Cleanup any global settings or check created data
    case 'cleanup':
    break;
  }
}
