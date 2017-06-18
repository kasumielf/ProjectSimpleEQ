using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Objects;

public class BasePlayer : MonoBehaviour {
    public TextMesh name;
    public Objects.Object baseObject = new Objects.Object();

	// Use this for initialization
	void Start () {
        name.text = baseObject.name + "(" + baseObject.id + ")";
	}
	
	// Update is called once per frame
	void Update () {
        transform.position = new Vector3(300 - this.baseObject.x, 0.0f, this.baseObject.y);
	}
}
