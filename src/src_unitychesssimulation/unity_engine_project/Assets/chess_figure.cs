using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class chess_figure : MonoBehaviour
{
    public float speed = 01.0f;
    public chess_board_generator CBG;
    // Start is called before the first frame update
    void Start()
    {
        
    }




    public Vector3 dest_pos;
    public bool en_move;
    public bool destiantion_reached = true;


    public void init(chess_board_generator _cbg) {
        CBG = _cbg;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (!en_move) { return; }

        this.transform.position = Vector3.Lerp(this.transform.position, dest_pos, speed * Time.deltaTime);

        if (Vector3.Distance(this.transform.position, dest_pos) < 0.5) {
            en_move = false;
            destiantion_reached = true;
        }
    }


    public void set_to_wp(waypoint _wp) {
        this.transform.position = new Vector3(_wp.pos_x, 0.0f, _wp.pos_y);
    }

    public void set_to_tile(chess_tile _t) {
        set_to_wp(_t.get_center_waypoint());
    }

    public void move_to_waypoint(waypoint _a, waypoint _b) {

        set_to_wp(_a);
        dest_pos = new Vector3(_b.pos_x, 0.0f, _b.pos_y);
        en_move = true;
        destiantion_reached = false;
    }


    public void move_to_tile(chess_tile _a, chess_tile _b) {
        //start und ziel ist jeweils der center waypoint
        waypoint start = _a.get_center_waypoint();
        waypoint end = _b.get_center_waypoint();


        //chess board hält alle nachbarn
        //direkstra drauf anwenden und lokal in einer liste speichern
        //alle wegpunkte in der liste abarbeiten
       

    }
}
