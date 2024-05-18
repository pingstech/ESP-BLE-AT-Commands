# ESP32 ile BLE Kullanımı

Bugünkü yazımda **ESP32**'ye **UART** üzerinde **AT komutları** göndererek **Bluetooth** özelliğini kullanarak bilgi alış-verişinin nasıl yapılacağını öğreneceğiz.

---
## 💡BLE AT Komutları Ve Parametre Ayarları

- #### 1️⃣AT+BLEINIT
&nbsp; &ensp;BLE'yi *<span style="color:orange">sunucu modunda çalıştırılması</span>* için gönderilmesi gereken komuttur. Aşağıdaki komut ESP32'ye gönderilerek BLE sunucu olarak çalıştırılması sağlanacaktır.

```c
AT+BLEINIT=2 
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*
   
```c
   OK
```
---
- #### 2️⃣AT+BLEADVDATAEX
&nbsp; &ensp;BLE’nin *<span style="color:orange">görünürlük ve parametre ayarlarının</span>* yapılabilmesi için ESP32’ye gönderilmesi gereken komuttur.
	 *<span style="color:green">Device Name:</span>* Cihaza verilecek isim
	 *<span style="color:green">Universal Unique Identifier:</span>* cihazın benzersiz bir şekilde tanımlanmasını sağlayan 128-bitlik evrensel bir kimlik numarasıdır. 
	 *<span style="color:green">Manufacturer Data:</span>* Üretici verisi (opsiyoneldir), 16'lık formatta.
	 *<span style="color:green">TX_Power_Option:</span>* TX güç seviyesinin eklenip eklenmeyeceğini belirtir. 1: Eklenir, 0: Eklenmez.

```c
AT+BLEADVDATAEX="Device_Name","Universal_Unique_Identifier","Manufacturer_Data",TX_Power_Option
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*
   
```c
   OK
```
----
- #### 3️⃣AT+BLEGATTSSRVCRE
&nbsp; &ensp;BLE'nin sunucu olarak çalıştırıldığında *<span style="color:orange">hizmetlerin aktif edilebilmesi</span>* için ESP32'ye gönderilmesi gereken komuttur.

```c
AT+BLEGATTSSRVCRE
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*
   
```c
   OK
```
---
- #### 4️⃣AT+BLEGATTSSRVSTART
&nbsp; &ensp;BLE'nin sunucu olarak çalıştırıldığında *<span style="color:orange">tüm hizmetlerin veya belirli bir hizmeti başlatmak</span>* için ESP32'ye gönderilmesi gereken komuttur.

```c
AT+BLEGATTSSRVSTART
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*
   
```c
   OK
```
---
- #### 5️⃣AT+BLEGATTSSRVSTART
&nbsp; &ensp;BLE'nin *<span style="color:orange">diğer cihazlar tarafından keşfedilebilmesi, yayının başlatılması</span>* için ESP32'ye gönderilmesi gereken komuttur.

```c
AT+BLEADVSTART
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*
   
```c
   OK
```
---
Yukarıda verilmiş olan tüm komutlar başarılı bir şekilde ESP32'ye gönderildiğin artık ESP32 BLE yayını yapmaya başlayacaktır. 

⚠️⚠️⚠️ Daha sonrasında ise bir bağlantının kurulup kurulmadığını kontrol etmemiz gerekmektedir. *<span style="color:red">Bir bağlantı kurulması durmunda</span>* aşağıdaki mesaj ESP32 tarafından bize iletilecektir.

```c
+BLECONN:
```

 ⚠️⚠️⚠️ *<span style="color:red">Eğer bağlantı sonlandırılırsa</span>* aşağıdaki mesaj ESP32 tarafından bize iletilecektir.

```c
+BLEDISCONN:
```

- #### 📫AT+BLEGATTSIND
	*<span style="color:orange">Bağlantının sağlandığı cihaza mesaj gönderilebilmesi</span>* için aşağıdaki komut ESP32'ye gönderilmelidir. 
	* *<span style="color:green">Connection Index:</span>* Mevcut bağlantıların listesinden hangi bağlantıya gönderileceğini belirtir..
	* *<span style="color:green">Service Index:</span>* Sunucu tarafından sunulan hizmetlerin listesinden hangisinin kullanılacağını belirtir. 
	* *<span style="color:green">Characteristic Index:</span>* Belirli bir hizmet içindeki karakteristiklerin listesinden hangisinin kullanılacağını belirtir.
	* *<span style="color:green">Data Length:</span>* Gönderilecek verinin uzunluğudur.

```c
AT+BLEGATTSIND=Connection_Index,Service_Index,Characteristic_Index,Data_Length
```

   Bu komuta karşılık olarak ESP32'nin aşağıdaki *<span style="color:red">mesajı geri dönmesi beklemeliyiz!</span>*

```c
   >
```

---

## 🚩Uygulama Gerçekleştirilmesi

&nbsp; &ensp;Yukarıda anlatılmış olan komutları bir işlemci aracılığı ile ESP32'ye gönderecek olan uygulamanın çalıştırılması için yapılması gereken işlemler anlatılacaktır. Burada yazılmış olan uygulama daha önceden yazılmış olan TCP Client projesine ek olarak yapılmıştır bundan dolayı "ESP32 Enable/Disable" fonksiyonları yer almamaktadır. 

⚠️ İsterseniz ESP32'ye hard reset atmak isterseniz ESP32'ye güç verilen ilgili pini belli bir süre LOW'a çekip daha sonrasında HIGH'a çekmeniz yeterli olacaktır. ⚠️

❗"ble_app.h" incelendiğinde init, polling, timer ve receive fonksiyonları ile karşılaşacaksınız. Bu fonksiyonlar muhakkak ilgili yerlerde çağırılmalıdır. ❗


#### 📣Tanımlanması Gerken Pointer Fonksiyonlar
- Görüntülünebilirlik:

&nbsp; &ensp;ESP32 başka cihazlar tarafından görüntülünebilir duruma geldiğinde bu fonksiyon çalışacaktır.

```c
static void device_detectable_fp(void * ble_obj, void * reference_obj);
```
---
- Bağlantının Sağlanması:

&nbsp; &ensp;ESP32'ye başka cihazlar tarafından bağlantı sağlandığında bu fonksiyon çalışacaktır.

```c
static void ble_connection_success_fp(void * ble_obj, void * reference_obj);
```
---
- Bağlantının Kesilmesi:

&nbsp; &ensp;ESP32'ye başka cihazlar tarafından bağlantı sağlandındaktan sonra bu bağlantı kesilirse bu fonksiyon çalışacaktır.

```c
static void ble_connection_fail_fp(void * ble_obj, void * reference_obj);
```
---
- Reset:

&nbsp; &ensp;ESP32 gönderilmiş olan komutları başarılı bir biçimde gönderemediyse ve yeterince denemede bulunduysa, "AT+RESTORE" komutunun gönderilmesini ya da ESP32'ye reset atılmasının beklenileceği fonksiyondur. 

```c
static void ble_reset_waiting_fp(void * ble_obj, void * reference_obj);
```
---
- Veri Gönderimi:

&nbsp; &ensp; ESP32'ye UART üstünden mesaj gönderilebilmesi için işlemcinin UART mesaj gönderme fonksiyonu bu fonksiyonda tanımlanmalıdır.

```c
static void ble_transmit_data_fp(void * ble_obj, void * reference_obj, const unsigned char * tx_data, unsigned int tx_data_length);
```
---
- Veri Alımı:

&nbsp; &ensp; Bağlantının sağlandığı cihaz bir mesaj gönderildiğinde bu fonksiyon çalışacaktır. Bu fonksiyon içerisinde alınan mesaj ve boyutu dönmektedir. Kullanıcı burada buffer'ını doldurmalıdır.

```c
static void ble_received_data_fp(void * ble_obj, void * reference_obj, const unsigned char * rx_data, unsigned int rx_data_length);
```
---

#### ❗Not:
&nbsp; &ensp; BLE yayınının yapılmasını sağlayan "state machine" yapısında "AT+RESTORE" komutu gönderilmemesinin sebebi bu projenin TCP Driver ve TCP Server uygulamaları ile birleştirilecek olmasıdır. Bundan dolayı bu projede bu komutun gönderilmesine yer verilmektense, komutun istenildiği biçimde gönderilmemesi durumunda "Reset" başlığı altında yer verilen callback fonksiyonu çalışacaktır!
