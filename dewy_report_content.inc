<?php

function dewy_report() {
    // If PHP stops execution at anything under 10 minutes, attempt to set time to 10 minutes
    if (ini_get('max_execution_time') > 0 && ini_get('max_execution_time') < 600) {
        set_time_limit(600);
    }

    // Measure benchmark
    $time_start = microtime(true);

    // Open stream
    $stream = new SplFileObject('php://output', 'w');
    $stream->fwrite('{');

    // Get all text fields on the site
    $text_fields = array();
    $fields = field_info_fields();
    foreach ($fields as $field) {
        if ($field['module'] == 'text') {
            array_push($text_fields, $field);
        }
    }
    unset($fields);

    // Get all node content
    $stream->fwrite('"nodes":{');
    $result = db_query("SELECT nid FROM node");
    $first_row = TRUE;
    while ($node = $result->fetchAssoc()) {
        if (!$first_row) { 
            $stream->fwrite(',');
        } else {
            $first_row = FALSE;
        }
        // See what text field data is associate with this node
        $content = array();
        $field_query = db_select('node', 'n');
        foreach ($text_fields as $field) {
            $table = _field_sql_storage_tablename($field);
            $column = _field_sql_storage_columnname($field['field_name'], 'value');
            $field_query->leftJoin($table, NULL, "$table.entity_id = n.nid");
            $field_query->fields(NULL, array($column));
        }
        $field_query->condition('n.nid', $node['nid'], '=');
        $field_result = $field_query->execute();
        if ($field = $field_result->fetchAssoc()) {
            foreach ($field as $key => $value) {
                if ($value) {
                    $content[$key] = $value;
                }
            }
        }
        $stream->fwrite(json_encode($node['nid']).':'.json_encode($content));
    }
    $stream->fwrite('},');
    unset($text_fields);

    // Get all block content
    $stream->fwrite('"blocks":{');
    $result = db_select('block_custom', 'block')
    ->fields('block', array('bid', 'body'))
    ->execute();
    $first_row = TRUE;
    while ($block = $result->fetchAssoc()) {
        if (!$first_row) { 
            $stream->fwrite(',');
        } else {
            $first_row = FALSE;
        }
        $stream->fwrite(json_encode($block['bid']).':'.json_encode($block['body']));
    }
    $stream->fwrite('}');

    // Report benchmark
    $time_end = microtime(true);
    $time = $time_end - $time_start;
    $stream->fwrite('"benchmark":'.$time);

    // Close stream
    $stream->fwrite('}');
}