function xmlToString(xmlData) {
    var xmlString;
    //IE
    if (window.ActiveXObject) {
        xmlString = xmlData.xml;
    }
    // code for Mozilla, Firefox, Opera, etc.
    else {
        xmlString = (new XMLSerializer()).serializeToString(xmlData);
    }
    return xmlString;
}

function __helper__svg_xml_to_string(_str){

            if(__helper__isArray(_str) || __helper__isHTMLCollection(_str)){
                var tmp_result = "";
                for(var i =0; i < _str.length;i++){
                    tmp_result += __helper__svg_xml_to_string(_str[i]);
                }
                return tmp_result;
            }

            return new XMLSerializer().serializeToString(_str);
        }







function IsJsonString(str) {
    try {
        JSON.parse(str);
    } catch (e) {
        return false;
    }
    return true;
}

function __helper__isArray(what) {
    return Object.prototype.toString.call(what) === '[object Array]';
}

function __helper__isHTMLCollection(what){
    return Object.prototype.toString.call(what) === '[object HTMLCollection]';

}


function __helper__fixedEncodeURIComponent (str) {
  return encodeURIComponent(str).replace(/[!'()*]/g, function(c) {
    return '%' + c.charCodeAt(0).toString(16);
  });
}