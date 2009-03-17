(function (){
    var links = new Array;
    var it = document.evaluate('//link[@rel="alternate"]', document, null, XPathResult.ANY_TYPE, null);
    var link = it.iterateNext();
    while (link) {
        var obj = new Object;
        obj.title = link.title;
        obj.href = link.href;
        obj.type = link.type;
        links.push(obj);
        link = it.iterateNext();
    }
    return links;
})();
