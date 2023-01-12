# TemperatureLoger
ESP8266과 Google Spread Sheet를 이용한 온도 Logger

# 테스트
SpreadSheet URL : https://docs.google.com/spreadsheets/d/1CrVGCopXGvlORy0PRAvVPKw5m6P105xAls0RzNh8bZw/edit#gid=0
값 업로드 테스트 : https://script.google.com/macros/s/AKfycbywrZNlmWRzi7_hcrFNk3I1j0weSsfOsqXoz2KBOJQeJGFRjSRrFs9_cl7zzTw4Wc6wXA/exec?temperature=33&deviceid=hehe1


# 아두이노 개발환경 설정방법
1. 파일->환경설정-> 추가적인 보드 URL-> http://arduino.esp8266.com/stable/package_esp8266com_index.json 기입
2. 툴->보드->보드매니저->contributed선택->esp8266설치(*반드시 2.7.4 버전 설치*)
3. 툴->보드->ESP8266 boards(2.7.4)->NodeMCU 1.0 선택
4. 툴->라이브러리 관리->DallasTemperature 검색 후 설치
