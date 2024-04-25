# Stacco
A playful instrument based on magnets
List of Materials:

- 4mm thick MDF panel to laser cut for the body.
- Bela MINI starter Kit (https://eu.shop.bela.io/products/bela-mini-starter-kit). This is if you want to do embedded synthesis. Alternatively, you can use any ESP32 microcontroller (using the ESP32_Code file) and read the serial from your computer. If you opt for the latter, you will need to custom buid a few 3d print components, as no audio in or out not mic will be needed.
- 8 30x4mm neodymium magnets, such as these: https://www.amazon.it/Wukong-Neodimio-Calamite-Magnetica-Frigorifero/dp/B098T1HBNV/ref=pd_sbs_d_sccl_3_4/260-9774401-7534231?pd_rd_w=g1yp3&content-id=amzn1.sym.27802479-1929-4933-824a-0b9d19995b35&pf_rd_p=27802479-1929-4933-824a-0b9d19995b35&pf_rd_r=46BGXGKP754MA91EB3X8&pd_rd_wg=NTK8B&pd_rd_r=70bc0d76-fc7f-491a-bcb4-d8f268a4806b&pd_rd_i=B098T1HBNV&psc=1
- 4 Taidacent MLX90393 (the Adafruit version works fine, but you will need to change the sensor's 3D model to accommodate it)
- 5x 4pin JST connectors (male and female)
- Stripboard 
- 1x electret mic (from Adafruit). You might want to skip this, as it is not a feature we're currently using.

  
