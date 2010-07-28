<?php
/**
 * Component for updating Nat-NG configuration.
 * 
 * Tested with NAT-ng version 6.x-1.3. If using this version of NAT-ng then ensure you have applied the patch from http://drupal.org/node/867434.
 * 
 * @author Ramesh Nair (hiddentao)
 */


function nat_ng_patterns($op, $id = null, &$data = null) {
  switch($op) {
    // Return the valid tags that this component can prepare and process
    case 'tags':
      return array('nat_ng');
    break;

    // Return a list of forms/actions this component can handle
    case 'actions':
      return array(
        'nat_ng_settings_form' => t('Configure Nat-NG for a node type'),
      );
    break;

    // Return a summary of an action
    case 'summary':
      if ('nat_ng_settings_form' == $id && isset($data['type'])) {
        return t('Configuring NAT-ng for node type: %name', array('%name' => $data['type']));
      }
    break;

    // Prepare data for processing
    case 'prepare':
    	
    	if (isset($data['vocabs']) && isset($data['vocabs']['vocab'])) {
    		$data['vocabs'] = array($data['vocabs']['vocab']);
    	}
    	
    break;

    // Pre validate actions
    case 'pre-validate':
    	if (!module_exists('nat_ng')) {
    		return t('Nat-NG module must be enabled.');
    	}
      if (!isset($data['type'])) {
        return t('You must specify a node "type" to configure.');
      }
    break;

    // Return the form_id('s) for each action
    case 'form_id':
    	return 'nat_ng_settings_form';
    	
    break;

    // Prepare for valid processing of this type of component
    case 'build':
    	if ('nat_ng_settings_form' == $id) {
        
    		module_load_include('inc','nat_ng','nat_ng.admin');
    		
    		// node type
        $types = node_get_types('types');
        $type_internal_name = '';
        foreach ($types as $type => $info) {
        	if ($info->name == $data['type']) {
        		$type_internal_name = $type;
        		break;
        	} else if ($type == $data['type']) {
            $type_internal_name = $type;
            break;
        	}
        }    		
        unset($data['type']);
        
        // set vocab ids
	      if (isset($data['vocabs']) && !empty($data['vocabs'])) {
	        // construct vocabs array (name => vocab id)
          $all_vocabs = taxonomy_get_vocabularies();
	      	$fn = create_function('$info','return $info->name;');
	        $all_vocabs = array_flip(array_map($fn,$all_vocabs));
	        
	        // save selected vocab ids to form data
	        $data['types'][$type_internal_name] = array_values(array_intersect_key($all_vocabs,array_flip($data['vocabs'])));
	      }
        unset($data['vocabs']);
	      
        // additional config
        $flags = array(
          'associate-node-body-with-term-description' => 'body',
          'delete-node-if-term-deleted' => 'delete',
          'delete-natng-node-if-natng-term-deleted' => 'delete_node',
          'allow-users-to-define-synonyms' => 'related',
          'natng-term-in-node-view-points-to-node' => 'node_links',
        );
        foreach ($flags as $f => $k) {
        	$data[$k][$type_internal_name] = isset($data[$f]) && 1 == $data[$f] ? 1 : 0;
        	unset($data[$f]); 
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
    	
    	// check node type
    	$types = node_get_types('types');
    	$found = FALSE;
    	foreach ($types as $type => $info) {
    		if ($type == $data['type'] || $info->name == $data['type']) {
    			$found = TRUE;
    			break;
    		}
    	}
    	if (!$found) {
    		return t('Invalid node type: %type', array('%type' => $data['type']));
    	}
    	
    	// check vocab names
    	if (isset($data['vocabs']) && !empty($data['vocabs'])) {
	      $all_vocabs = taxonomy_get_vocabularies();
	      $fn = create_function('$info','return $info->name;');
	      $all_vocab_names = array_flip(array_map($fn, $all_vocabs));
	      $found = array();
	      foreach ($data['vocabs'] as $vocab) {
          if (!isset($all_vocab_names[$vocab]))
          	$found[] = $vocab;
	      }
	      if (!empty($found))
	        return t('Invalid vocab names: %names', array('%names' => implode(', ',$found)));
    	}
    	
    break;

    // Build a patterns actions and parameters
    case 'params':
      return array();
    break;

  }
}

