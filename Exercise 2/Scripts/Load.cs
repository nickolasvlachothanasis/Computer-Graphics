using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class Load : MonoBehaviour
{
    public GameObject Cube;
    public Material Red;
    public Material Green;
    public Material Blue;
    public Material T1;
    public Material T2;
    public Material T3;
    public Material Black;
    public int L = 0;
    public int N = 0;
    public int K = 0;
    public GameObject player;

    // Start is called before the first frame update
    void Start()
    {
        
        Vector3 initialPosition = new Vector3(-1, -1, -1);
        Vector3 playerPosition = initialPosition;
        // Read the file and display it line by line.
        System.IO.StreamReader file = new System.IO.StreamReader("file.maz");
        string line = "";
        line = file.ReadLine();
        string[] buf = line.Split('=');
        L = Int32.Parse(buf[1]);

        line = file.ReadLine();
        buf = line.Split('=');
        N = Int32.Parse(buf[1]);

        line = file.ReadLine();
        buf = line.Split('=');
        K = Int32.Parse(buf[1]);


        line = file.ReadLine();
        for(int j = 0; j < L; j++)
        {
            for (int i = 0; i < N; i++)
            {
                line = file.ReadLine();
                buf = line.Split(' ');
                for (int k = 0; k < N; k++)
                {
                    Vector3 position = new Vector3(2*i, 2*j, 2*k);
                    if(buf[k].Equals("E") == false)
                        CreateCube(buf[k], position);
                    else if(j == 0)
                    {
                        if(playerPosition == initialPosition)
                        {
                            playerPosition = position;
                            
                        }
                        else
                        {
                            if (UnityEngine.Random.Range(0, 10) < 5)
                            {
                                playerPosition = position;
                            }
                        }
                    }
                }
            }
            line = file.ReadLine();
        }

        player.transform.position = playerPosition;
        file.Close();

        Cube.GetComponent<Renderer>().material = Black;
        // Suspend the screen.
        Vector3 positionFloor = new Vector3(N, -2, N);
        Cube.transform.localScale = new Vector3(2 * N + 2, 1, 2 * N + 2);
        Instantiate(Cube, positionFloor, Quaternion.identity);

        Vector3 positionWall = new Vector3(N, L, -2);
        Cube.transform.localScale = new Vector3(2 * N + 2, 2*L+2, 2);
        Instantiate(Cube, positionWall, Quaternion.identity);

        positionWall = new Vector3(N, L, 2*N);
        Cube.transform.localScale = new Vector3(2 * N + 2, 2 * L + 2, 2);
        Instantiate(Cube, positionWall, Quaternion.identity);

        positionWall = new Vector3(-2, L, N);
        Cube.transform.localScale = new Vector3(2, 2 * L + 2, 2 * N + 2);
        Instantiate(Cube, positionWall, Quaternion.identity);

        positionWall = new Vector3(2 * N, L, N);
        Cube.transform.localScale = new Vector3(2, 2 * L + 2, 2 * N + 2);
        Instantiate(Cube, positionWall, Quaternion.identity);

        Destroy(Cube);
    }

    void CreateCube(string type, Vector3 position)
    {

        GameObject obj = Instantiate(Cube, position, Quaternion.identity);
        if (type.Equals("R"))
            obj.GetComponent<Renderer>().material = Red;
        else if (type.Equals("G"))
                    obj.GetComponent<Renderer>().material = Green;
        else if (type.Equals("B"))
                    obj.GetComponent<Renderer>().material = Blue;
        else if (type.Equals("T1"))
                    obj.GetComponent<Renderer>().material = T1;
        else if (type.Equals("T2"))
                    obj.GetComponent<Renderer>().material = T2;
        else if (type.Equals("T3"))
                    obj.GetComponent<Renderer>().material = T3;

    }
    // Update is called once per frame
    void Update()
    {
        
    }
}
