using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class chess_board_generator : MonoBehaviour
{
    public const int BOARD_SIZE_ROWS = 8;
    public const int BOARD_SIZE_COLS = 12;
    chess_tile[,] chessboard = new chess_tile[BOARD_SIZE_ROWS, BOARD_SIZE_COLS];

    public GameObject white_tile;
    public GameObject black_tile;
    public GameObject storage_tile;
    public GameObject chess_pciece;
   
    public float offset = 5.0f;

  
    public bool regernate = false;
    // Start is called before the first frame update

   
    public void generate_board() {
        //REMOVE OLD CHILDREN
        foreach (Transform child in transform)
        {
            GameObject.Destroy(child.gameObject);
        }
        //GENERATE BOARD
        int counter_ps = 0;
        int counter = 0;
        for (int i = 0; i < BOARD_SIZE_ROWS; i++)
        {
            for (int j = 0; j < BOARD_SIZE_COLS; j++)
            {

                GameObject go = null;
                if (j < 2 || j > 9)
                {
                    go = (GameObject)Instantiate(storage_tile, this.transform.position + new Vector3(i * offset, 0, j * offset), Quaternion.identity, this.transform);
                    go.name = "chesstile_storage_" + counter.ToString() + "_" + ((char)i) + (j + 1).ToString();
                }
                else if (counter_ps % 2 == 0)
                {
                    go = (GameObject)Instantiate(white_tile, this.transform.position + new Vector3(i * offset, 0, j * offset), Quaternion.identity, this.transform);
                    go.name = "chesstile_white_" + counter.ToString() + "_" + ((char)i) + (j + 1).ToString();
                    counter_ps++;
                }
                else if (counter_ps % 2 == 1)
                {
                    go = (GameObject)Instantiate(black_tile, this.transform.position + new Vector3(i * offset, 0, j * offset), Quaternion.identity, this.transform);
                    go.name = "chesstile_black_" + counter.ToString();
                    counter_ps++;
                }
                else {
                    counter_ps++;
                }


                //INIT THE TILE
                go.GetComponent<chess_tile>().init(counter,(chess_tile.COLOR)(counter_ps % 2),(chess_tile.ROW)i,(chess_tile.COL)j);
                //SAVE THE TILE
                chessboard[i, j] = go.GetComponent<chess_tile>();
                counter++;
            }
            counter_ps++;
        }



        //GENERATE WAYPOINT LIST
        //-> jedes tile hat einen wegpunkt
        //-> nun die




        //GENERATE SAMPLE PICESE MOVEMENT
        GameObject po = (GameObject)Instantiate(chess_pciece);
        po.GetComponent<chess_figure>().init(this);
        //TEST MOVE TO A GIVEN WAYPOINTS
        //po.GetComponent<chess_figure>().move_to_waypoint(chessboard[0, 2].get_center_waypoint(), chessboard[6, 10].get_center_waypoint());
        //TEST
        po.GetComponent<chess_figure>().move_to_tile(chessboard[0, 2], chessboard[6, 10]);

    }






    void Start()
    {
        generate_board();
    }

    // Update is called once per frame
    void Update()
    {
        if (regernate) {
            regernate = false;
            generate_board();
        }
    }
}
