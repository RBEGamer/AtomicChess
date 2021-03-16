using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class waypoint
{
    public int id;
    public float pos_x;
    public float pos_y;

    public chess_tile tile;

    public waypoint(int _id, chess_tile _tile) {
        tile = _tile;
        id = _id;
        //TODO CLAC POSITION
    }


}
