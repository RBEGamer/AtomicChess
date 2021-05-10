using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class chess_tile : MonoBehaviour
{
    public chess_figure figure;
    public enum COLOR
    {
        BLACK = 0,
        WHITE = 1,
        STORAGE = 2
    }


    public enum ROW {
        ROW_1 = 0,
        ROW_2 = 1,
        ROW_3 = 2,
        ROW_4 = 3,
        ROW_5 = 4,
        ROW_6 = 5,
        ROW_7 = 6,
        ROW_8 = 7
    }
    public enum COL
    {
        COL_STORE_1 = 0,
        COL_STORE_2 = 1,
        COL_A = 2,
        COL_B = 3,
        COL_C = 4,
        COL_D = 5,
        COL_E = 6,
        COL_F = 7,
        COL_G = 8,
        COL_H = 9,
        COL_STORE_3 = 10,
        COL_STORE_4 = 11,
    }

    public waypoint wp_tl;
    public waypoint wp_tr;
    public waypoint wp_bl;
    public waypoint wp_br;
    public waypoint wp_center;


    public COLOR color;
    public ROW row;
    public COL col;
    public int tile_id;
    public void init(int _tile_id, COLOR _color, ROW _row, COL _col) {
        color = _color;
        row = _row;
        col = _col;
        tile_id = _tile_id;

        //GENERATE WAYPOINTS ID

        wp_tl = new waypoint(tile_id*5 + 0, this);
        wp_tr = new waypoint(tile_id*5 + 1, this);
        wp_bl = new waypoint(tile_id*5 + 2, this);
        wp_br = new waypoint(tile_id*5 + 3, this);
        wp_center = new waypoint(tile_id * 5 + 3, this);
        //CALC POSTIONS FOR EACH WP
        wp_center.pos_x = this.gameObject.transform.position.x;
        wp_center.pos_y = this.gameObject.transform.position.z;


    }

    public waypoint get_center_waypoint() {
        return wp_center;

    }
    
    //holt nächsten wegpunkt zum jetigen = kleinster abstand
    //mittels destiantion tile einen pfad um das aktuelle tile herum erstellen
    List<waypoint> get_waypoints_to_next_tile(waypoint _current_waypoint, chess_tile _destination_tile) {
        return null;
    }

    public List<waypoint> get_waypoints_out_of_tile(chess_tile _destination_tile) {
        return null;
    }

    

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
