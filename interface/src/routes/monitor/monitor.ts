//based on https://github.com/jasonsturges/threejs-sveltekit/blob/main/src/lib/scene.ts
// also check https://github.com/threlte/threlte

import {
    BoxGeometry, SphereGeometry,
    // DirectionalLight,
    // HemisphereLight,
    Mesh,
    MeshStandardMaterial, MeshBasicMaterial, Color,
    PerspectiveCamera,
    Scene,
    WebGLRenderer
  } from 'three';

  const scene = new Scene();
  
  const camera = new PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 500);
  camera.position.z = 5;
//   camera.lookAt(5, 5, 5);
  
  const geometry = new BoxGeometry();
  
  const material = new MeshStandardMaterial({
      color: 0x00ff00,
      metalness: 0.132
  });
  
//   const cube = new Mesh(geometry, material);
//   scene.add(cube);


import { OrbitControls } from 'three/addons/controls/OrbitControls.js'

//   import { TextGeometry } from 'three/addons/geometries/TextGeometry.js';
//   import { FontLoader } from 'three/addons/loaders/FontLoader.js';

// let textMesh:Mesh;
//   const loader = new FontLoader();
//     loader.load( 'https://raw.githubusercontent.com/mrdoob/three.js/refs/heads/dev/examples/fonts/helvetiker_regular.typeface.json', function ( font ) {

//     console.log(font)
// 	const geometry = new TextGeometry( 'MoonModules', {
// 		font: font,
//         size: 2,
//         height: 0.1,
//         curveSegments: 32,
//         bevelEnabled: true,
//         bevelThickness: 0.03,
//         bevelSize: 0.02,
//         bevelOffset: 0,
//         bevelSegments: 4,		// size: 10,
// 		// depth: 5,
// 		// curveSegments: 12,
// 		// bevelEnabled: true,
// 		// bevelThickness: 1,
// 		// bevelSize: 8,
// 		// bevelOffset: 0,
// 		// bevelSegments: 5
// 	} );
//     console.log("geometry", geometry);
//     textMesh = new Mesh(geometry, material);
//     scene.add(textMesh);
// }
// ,function ( xhr ) {
// 		console.log( (xhr.loaded / xhr.total * 100) + '% loaded' );
// 	},

// 	// onError callback
// 	function ( err ) {
// 		console.log( 'An error happened' );
// 	} );
  
  // const directionalLight = new DirectionalLight(0x9090aa);
  // directionalLight.position.set(-10, 10, -10).normalize();
  // scene.add(directionalLight);
  
  // const hemisphereLight = new HemisphereLight(0xffffff, 0x444444);
  // hemisphereLight.position.set(1, 1, 1);
  // scene.add(hemisphereLight);
  
  let renderer:WebGLRenderer;

  let canvas:HTMLCanvasElement;
  
  const animate = () => {
      requestAnimationFrame(animate);
    //   if (textMesh) {
    //     textMesh.rotation.x += 0.01;
    //     textMesh.rotation.y += 0.01;
    //   }
    // scene.rotation.x += 0.01;
    // scene.rotation.y += 0.01;
    // scene.rotation.z += 0.01;
      renderer.render(scene, camera);
  };
  
  const resize = () => {
        if (canvas.parentNode && canvas.parentNode.clientWidth && canvas.parentNode.clientHeight) {
        // console.log("resizing",canvas.parentNode.clientWidth, canvas.parentNode.clientHeight)
      renderer.setSize(canvas.parentNode.clientWidth, canvas.parentNode.clientHeight);
      camera.aspect = canvas.parentNode.clientWidth / canvas.parentNode.clientHeight;
        }
      camera.updateProjectionMatrix();
  };
  
  // https://stackoverflow.com/questions/30359830/how-do-i-clear-three-js-scene
  function clearThree(obj: any){
    while(obj.children.length > 0){ 
      clearThree(obj.children[0]);
      obj.remove(obj.children[0]);
    }
    if(obj.geometry) obj.geometry.dispose();
  
    if(obj.material){ 
      //in case of map, bumpMap, normalMap, envMap ...
      Object.keys(obj.material).forEach(prop => {
        if(!obj.material[prop])
          return;
        if(obj.material[prop] !== null && typeof obj.material[prop].dispose === 'function')                                  
          obj.material[prop].dispose();                                                      
      })
      obj.material.dispose();
    }
  } 

  export const createScene = (el:HTMLCanvasElement) => {
      canvas = el;
      // scene.clear();
      clearThree(scene)

      renderer = new WebGLRenderer({ antialias: true, canvas: el });
      renderer.setClearColor( 0xffffff, 0); //clear background see https://stackoverflow.com/questions/16177056/changing-three-js-background-to-transparent-or-other-color

      let controls = new OrbitControls( camera, renderer.domElement );

      resize();
      animate();
  };
  
  export const addLed = (size: any, x: any, y: any, z:any) => {
    // console.log("adding led", size, x, y, z);
    let geometry = new SphereGeometry(size);
    const material = new MeshBasicMaterial({transparent: true, opacity: 1.0});
    material.color = new Color(0.5,0,0.5);
    const mesh = new Mesh( geometry, material );
    mesh.position.set(x, y, z);
    // mesh.name = outputsIndex + " - " + ledsIndex++;
    scene.add( mesh );
    };

    // export const colorLed = (x: any, y: any, z:any, r: any, g: any, b:any) => {
    export const colorLed = (ledIndex:any, r: any, g: any, b:any) => {
        // console.log("coloring led", x, y, z, r, g, b);
        // let ledIndex = x + y*10 + z * 100;
        //assuming all childred are Spheres/leds
        if (scene.children[ledIndex] && scene.children[ledIndex].material) {
            if (r+g+b<1)
                scene.children[ledIndex].material.opacity = r+g+b;//.05;
            else {
                scene.children[ledIndex].material.opacity = 1;
            }
            scene.children[ledIndex].material.color = new Color(r, g, b);
        }
    };

  window.addEventListener('resize', resize);