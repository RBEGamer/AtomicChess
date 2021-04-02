module.exports = {
    rank_to_elo_rating_name: function (_rank) {
        if (_rank >= 2700) {return "SUPER GRANDMASTER";}
        else if(_rank >= 2500){return "MOST GRANDMASTERS";}
        else if(_rank >= 2400){return "MOST INTERNATIONAL MASTERS";}
        else if(_rank >= 2300){return "FIDE MASTERS";}
        else if(_rank >= 2200){return "FIDE CANDIDATE MASTERS";}
        else if(_rank >= 2000){return "CANDIDATE MASTERS";}
        else if(_rank >= 1800){return "CLASS A";}
        else if(_rank >= 1600){return "CLASS B";}
        else if(_rank >= 1400){return "CLASS C";}
        else if(_rank >= 1200){return "CLASS D";}
        else{return "NOOB";}
    }
}