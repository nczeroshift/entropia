function convertSecondsToTimeString(time){
    var minutes = Math.floor(time / 60.0);
    var secs = time - minutes * 60;
    var seconds = Math.floor(secs);
    var mili = Math.floor(1000 * (secs - seconds))
    function pad(val){
        if(val < 10)
            return "0"+val;
        return ""+val;
    }
    return pad(minutes) + ":" + pad(seconds) + ":" + pad(mili);
}

function wndTimeline(wnd){
    this._pWindow = wnd;
    this._pTimeline = new Timeline(this._pWindow.find(".tl"));

    this._pTimeline.setChangeCallback(function (id, t, dur, track) {
        console.log(id, t, dur, track);
    });
    
    this._pTimeline.setTimeChangeCallback(function(start, duration){
        $(".tlFromTxt").val(convertSecondsToTimeString(start));
        $(".tlToTxt").val(convertSecondsToTimeString(start+duration));
    });
    
    this._pFetchTimeline();
    
    this._pTimeline.resize();
}

wndTimeline.prototype._pHandleTimelineData = function(data){
    if(data["timeline"] != null){
                
    }
    
    if(data["scenes"] != null){
        var dom = this._pWindow.find(".tlScenesList");
        
        for(var i = 0;i < data["scenes"].length; i++){
            var e = data["scenes"][i];
            
            var html = "<div class=\"tlSceneEntry\">";
            html += "<div class=\"typeId\" title=\"Type Id\">"+e.id+"</div>";
            html += "<div class=\"title\" title=\"Name Id\">"+e.name+"</div>";
            html += "<div class=\"delete\" title=\"Delete\"></div>";
            
            html += "</div>";
            
            dom.append($(html));
        }
        
        for(var i = 0;i< data["timeline"].length;i++){
            var e = data["timeline"][i];
            var start = e["start"];
            var end = e["end"];
            var name = e["name"];
            var track = e["track"];
            this._pTimeline.addSegment(name, start, end-start, track, name);
        }
    }
    
    this._pTimeline.resize();
};

wndTimeline.prototype._pFetchTimeline = function(){
    var scope = this;
    $.ajax({
        url:"/x/data.json?timeline="+(new Date()).getTime(),
        success: function(data){
            if(data!=null)
                scope._pHandleTimelineData(data);
        },
        error: function(data){
            
        }
    })
};

wndTimeline.prototype.resize = function(){
    this._pTimeline.resize();
}